#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// #include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "transformations.hpp"
#include "shaderProgram.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "framebuffer.hpp"

using namespace std;


ShaderProgram *teapotShaderPtr, *planeShaderPtr, *giantTrianglePtr;



int lastX{}, lastY{};

glm::mat4 rx(1), ry(1), tz(1), transformation_in_camera_frame(1), reflection_matrix(1);
glm::mat4 rx_plane(1), ry_plane(1), tz_plane(1), transformation_in_camera_frame_plane(1);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keybCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

int main() {

    cout << "am i here?" << endl;
    if(!glfwInit()) {
        cout << "glfw could not be initialized. " << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* windowPtr = glfwCreateWindow(600, 600, "render to texture", nullptr, nullptr);
    if(!windowPtr) {
        cout << "failed to create glfw window. " << endl;
        return -2;
    }
    glfwMakeContextCurrent(windowPtr);
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
    glfwSetKeyCallback(windowPtr, keybCallback);
    glfwSetMouseButtonCallback(windowPtr, mouseCallback);
    glfwSetScrollCallback(windowPtr, scroll_callback);
    glfwSetCursorPosCallback(windowPtr, cursor_position_callback);

    // const char* version = (const char*)glGetString(GL_VERSION);
    // std::cout << "OpenGL Version: " << version << std::endl;

    // glew crashed suddenly. i could not solve. i moved to glad.
    // glewExperimental = GL_TRUE;
    // if(glewInit()) { //!= GLEW_OK) {
    //     cout << "glew could not be initialized. " << endl;
    //     cout << "GLEW--->INITIALIZATION::ERROR " << endl;
    //     cout << glewGetErrorString(glewInit()) << endl;
    //     cout << glewInit() << endl;
    //     return -3;
    // }

    // glad init;
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "failed to initialize glad" << endl;
        return -1; 
        glfwTerminate();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(windowPtr, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");


    glClearColor(0.1, 0.1, 0.1, 1);
    glEnable(GL_DEPTH_TEST);


    ShaderProgram teapotShader("../glsl/teapotVS.vert", "../glsl/teapotFS.frag");
    teapotShader.useProgram();
    teapotShaderPtr = &teapotShader;

    // Mesh teapotMesh("../objFiles/sphere.obj");
    Mesh teapotMesh("../objFiles/teapot.obj");
    teapotMesh.myTextureLoader("../images/brick.png", "texture_diffuse");

    Camera camera(glm::vec3(0, 0.5, 2.4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 view = camera.getLookAtMatrix();
    glm::mat4 proj = perspectiveProjection_constNear(25.0f, 1.0f, -1.0f, -1000.0f);

    teapotShader.setMat4("projection", proj);
    teapotShader.setMat4("view", view);
    // program.setMat4("model", glm::mat4(1));
    // teapotMesh.centerTheMesh();
    teapotShader.setMat4("model", teapotMesh.getModelMatrix());
    teapotShader.setMat4("transformation_in_camera_frame", transformation_in_camera_frame);
    reflection_matrix = reflectOverPlaneMatrix(0, 1, 0, 0);
    teapotShader.setMat4("reflection_matrix", reflection_matrix);


    ShaderProgram planeShader("../glsl/planeVS.vert", "../glsl/planeFS.frag");
    planeShader.useProgram();
    planeShaderPtr = &planeShader;

    Mesh planeMesh("../objFiles/plane_2.obj");
    planeMesh.transformMesh(glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(1, 0, 0)));
    planeMesh.transformMesh(glm::scale(glm::mat4(1), glm::vec3(2.0f)));
    planeShader.setMat4("model", planeMesh.getModelMatrix());
    planeShader.setMat4("transformation_in_camera_frame", transformation_in_camera_frame);
    planeShader.setMat4("view", view);
    planeShader.setMat4("projection", proj);


    //////////////////////////////////////////

    ShaderProgram giantTriangleShader("../glsl/environmentMap.vert", "../glsl/environmentMap.frag");
    giantTriangleShader.useProgram();
    giantTrianglePtr = &giantTriangleShader;

    Mesh giantTriangleMesh("../objFiles/environmentMapPlane.obj");
    // Mesh giantTriangleMesh("../objFiles/giant_triangle.obj");
    string addres("../images/cubemap_");
    vector<string> paths;
    paths.push_back(addres+"posx.png"); 
    paths.push_back(addres+"negx.png"), 
    paths.push_back(addres+"posy.png"), 
    paths.push_back(addres+"negy.png"), 
    paths.push_back(addres+"posz.png"), 
    paths.push_back(addres+"negz.png");
    giantTriangleMesh.textureCubeMapLoader(paths, "environmentMap");
    giantTriangleShader.setMat4("u_invProj", glm::inverse(proj));
    giantTriangleShader.setMat4("u_invView", glm::inverse(view));



    GLuint environmentTextureUnitId = giantTriangleMesh.cubeMapTextureUnitId;

    planeShader.useProgram();
    planeShader.setInt("environmentMap", environmentTextureUnitId);
    planeShader.setVec2("textureResolution", glm::vec2(600, 600));

    teapotShader.useProgram();
    teapotShader.setInt("environmentMap", environmentTextureUnitId);



    Framebuffer fb(600, 600);
    fb.createTexture("renderedTextureId", GL_RGBA);
    fb.createDepthBuffer();
    fb.configureFramebuffer();


    planeShader.useProgram();
    planeShader.setInt(fb.getTexture().type, fb.getTexture().textureUnit);
    planeMesh.addTexture(fb.getTexture());


    // planeShader.setInt(fb.getTexture().type, fb.getTexture().textureUnit);
    // planeMesh.addTexture(fb.getTexture());



    while(!glfwWindowShouldClose(windowPtr)) {

        fb.setAsRenderTarget();
        glClearColor(0, 0, 0, 0); // teapot background color;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // drawing should be here.

        teapotShader.useProgram();
        teapotShader.setMat4("reflection_matrix", reflection_matrix);
        teapotMesh.draw(teapotShader);


        glGenerateTextureMipmap(fb.getTexture().id); // generate mipmap for rendered texture.

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, 600, 600);
        glClearColor(0, 0, 0, 1); // plane background color;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        teapotShader.useProgram();
        teapotShader.setMat4("reflection_matrix", glm::mat4(1));
        teapotMesh.draw(teapotShader);

        planeShader.useProgram();
        planeMesh.draw(planeShader);

        // this mask is for optimizing.
        glDepthMask(GL_FALSE); 
        giantTriangleShader.useProgram();
        giantTriangleMesh.draw(giantTriangleShader);
        glDepthMask(GL_TRUE);

        teapotShader.useProgram();


        // drawing ends.


        ///////////////////////// imgui things begin. ////////////////////////
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("imgui window for input for shading project");
        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ///////////////////////// imgui things end. ////////////////////////


        glfwSwapBuffers(windowPtr);
        glfwPollEvents();
    }

    ////// imgui terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw teriminate. 
    glfwDestroyWindow(windowPtr);
    glfwTerminate();

    return 0;
}

void keybCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(!ImGui::GetIO().WantCaptureKeyboard) {
        if(key == GLFW_KEY_CAPS_LOCK) {
            glfwSetWindowShouldClose(window, true);
            // cout << "demek ki tetikleniyormus callbackler. " << endl;
        }

    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if(!ImGui::GetIO().WantCaptureMouse) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // cout << "(x: "<< xpos <<  ", y: "<< ypos << ")" << endl;

        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            cout << "mouse sag buton tetiklendi. " << endl;
            lastX = (int)xpos;
            lastY = (int)ypos;
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            cout << "left one selammsss" << endl;
            lastX = (int)xpos;
            lastY = (int)ypos;
        }

    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if(!ImGui::GetIO().WantCaptureMouse) {

        cout << "y offset: " << yoffset << endl;
        static float fovY{25};
        static float fovY_plane{25};

        if(yoffset == 1) { // zoom in.

            
            if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                fovY_plane -= 0.5f;
                if(fovY_plane<=1) fovY_plane = 1.0f;
                glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);
                planeShaderPtr->useProgram();
                planeShaderPtr->setMat4("projection", proj_plane);

                giantTrianglePtr->useProgram();
                giantTrianglePtr->setMat4("u_invProj", glm::inverse(proj_plane));

            } else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { // set both plane and teapot.
                fovY_plane -= 0.5f;
                if(fovY_plane<=1) fovY_plane = 1.0f;
                glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);
                planeShaderPtr->useProgram();
                planeShaderPtr->setMat4("projection", proj_plane);

                giantTrianglePtr->useProgram();
                giantTrianglePtr->setMat4("u_invProj", glm::inverse(proj_plane));

                fovY -= (float)0.5f;
                if(fovY<1) fovY = 1.0f;
                glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
                teapotShaderPtr->useProgram();
                teapotShaderPtr->setMat4("projection", proj);

            } else {
                fovY -= (float)0.5f;
                if(fovY<1) fovY = 1.0f;
                glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
                teapotShaderPtr->useProgram();
                teapotShaderPtr->setMat4("projection", proj);

            }

        } else if(yoffset == -1) { // zoom out.


            if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                fovY_plane += 0.5f;
                if(fovY_plane>=88) fovY_plane = 88.0f;
                glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);
                planeShaderPtr->useProgram();
                planeShaderPtr->setMat4("projection", proj_plane);

                giantTrianglePtr->useProgram();
                giantTrianglePtr->setMat4("u_invProj", glm::inverse(proj_plane));

            } else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { // set both plane and teapot.

                fovY += (float)0.5f;
                if(fovY>=88) fovY = 88.0f;
                glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
                teapotShaderPtr->useProgram();
                teapotShaderPtr->setMat4("projection", proj);

                fovY_plane += 0.5f;
                if(fovY_plane>=88) fovY_plane = 88.0f;
                glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);
                planeShaderPtr->useProgram();
                planeShaderPtr->setMat4("projection", proj_plane);

                giantTrianglePtr->useProgram();
                giantTrianglePtr->setMat4("u_invProj", glm::inverse(proj_plane));

            } else {
                fovY += (float)0.5f;
                if(fovY>=88) fovY = 88.0f;
                glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
                teapotShaderPtr->useProgram();
                teapotShaderPtr->setMat4("projection", proj);

            }


        }

    }
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    if(!ImGui::GetIO().WantCaptureMouse) {
        int diffX = (int)x - lastX;
        int diffY = (int)y - lastY;

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // left mouse button. adjust rotation.
            
            // cout << "(x: "<< x <<  ", y: "<< y << ")" << endl;
            float rotate_y_axis{}, rotate_x_axis{};
            rotate_y_axis = (float)diffX/10.0f;
            rotate_x_axis = (float)diffY/10.0f;

            if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                rx_plane = glm::rotate(rx_plane, glm::radians(rotate_y_axis), glm::vec3(0, 1, 0));
                ry_plane = glm::rotate(ry_plane, glm::radians(rotate_x_axis), glm::vec3(1, 0, 0));
                rx = glm::rotate(rx, glm::radians(rotate_y_axis), glm::vec3(0, 1, 0));
                ry = glm::rotate(ry, glm::radians(rotate_x_axis), glm::vec3(1, 0, 0));
            } else if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                rx_plane = glm::rotate(rx_plane, glm::radians(rotate_y_axis), glm::vec3(0, 1, 0));
                ry_plane = glm::rotate(ry_plane, glm::radians(rotate_x_axis), glm::vec3(1, 0, 0));
            } else {
                rx = glm::rotate(rx, glm::radians(rotate_y_axis), glm::vec3(0, 1, 0));
                ry = glm::rotate(ry, glm::radians(rotate_x_axis), glm::vec3(1, 0, 0));
            }

        }

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // right mouse button. adjust translation.

            // cout << "(x: "<< x <<  ", y: "<< y << ")" << endl;
            float translate_z{};
            translate_z = (float)diffY/30.0f;

            if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                tz_plane = glm::translate(tz_plane, glm::vec3(0, 0, -translate_z));
                tz = glm::translate(tz, glm::vec3(0, 0, -translate_z));
            } else if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
                tz_plane = glm::translate(tz_plane, glm::vec3(0, 0, -translate_z));
            } else {
                tz = glm::translate(tz, glm::vec3(0, 0, -translate_z));
            }

        }

        transformation_in_camera_frame = tz * ry * rx;
        teapotShaderPtr->useProgram();
        teapotShaderPtr->setMat4("transformation_in_camera_frame", transformation_in_camera_frame);


        transformation_in_camera_frame_plane = tz_plane * ry_plane * rx_plane;
        planeShaderPtr->useProgram();
        planeShaderPtr->setMat4("transformation_in_camera_frame", transformation_in_camera_frame_plane);
        
        lastX = x; lastY = y;

    }
}