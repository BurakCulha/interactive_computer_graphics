#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "transformations.hpp"
#include "shaderProgram.hpp"
#include "mesh.hpp"
#include "camera.hpp"

using namespace std;



ShaderProgram *teapotShaderPtr, *planeShaderPtr;


int g_button{}, g_state{};
int lastX{}, lastY{};

glm::mat4 rx(1), ry(1), tz(1), transformation_in_camera_frame(1);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keybCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

int main() {

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

    if(glewInit() != GLEW_OK) {
        cout << "glew could not be initialized. " << endl;
        cout << "GLEW--->INITIALIZATION::ERROR " << endl;
        return -3;
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

    Mesh teapotMesh("../objFiles/teapot.obj");
    teapotMesh.myTextureLoader("../images/brick.png", "texture_diffuse");

    Camera camera(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 view = camera.getLookAtMatrix();
    glm::mat4 proj = perspectiveProjection_constNear(45.0f, 1.0f, -1.0f, -1000.0f);

    teapotShader.setMat4("projection", proj);
    teapotShader.setMat4("view", view);
    // program.setMat4("model", glm::mat4(1));

    teapotMesh.centerTheMesh();
    teapotShader.setMat4("model", teapotMesh.getModelMatrix());
    
    teapotShader.setMat4("transformation_in_camera_frame", transformation_in_camera_frame);

    ShaderProgram planeShader("../glsl/planeVS.vert", "../glsl/planeFS.frag");
    planeShader.useProgram();
    planeShaderPtr = &planeShader;

    Mesh planeMesh("../objFiles/plane_2.obj");

    planeShader.setMat4("model", planeMesh.getModelMatrix());
    planeShader.setMat4("transformation_in_camera_frame", transformation_in_camera_frame);
    planeShader.setMat4("view", view);
    planeShader.setMat4("projection", proj);
    


    while(!glfwWindowShouldClose(windowPtr)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // drawing should be here.
        teapotShader.useProgram();
        teapotMesh.draw(teapotShader);

        planeShader.useProgram();
        planeMesh.draw(planeShader);


        teapotShader.useProgram();

        // drawing ends.


        ///////////////////////// imgui things begin. ////////////////////////
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("imgui window for input for shading project");
        // // gui elements here.
        static glm::vec3 light_dir(1.0f, 1.0f, 1.0f);
        
        static glm::vec2 alpha_beta(0, 0);
        ImGui::SliderFloat2("light direction", &alpha_beta.x, -90, 90);
        light_dir.y = glm::sin(glm::radians(alpha_beta.x));
        light_dir.z = glm::cos(glm::radians(alpha_beta.x)) * glm::cos(glm::radians(alpha_beta.y));
        light_dir.x = glm::cos(glm::radians(alpha_beta.x)) * glm::sin(glm::radians(alpha_beta.y));
        teapotShader.setVec3("light_direction", light_dir);

        static glm::vec3 ambient_color(0.5), diffuse_color(0.5), specular_color(1);
        ImGui::SliderFloat3("ambient color: ", &ambient_color.x, 0, 1);
        ImGui::SliderFloat3("diffuse color: ", &diffuse_color.x, 0, 1);
        ImGui::SliderFloat3("specular color: ", &specular_color.x, 0, 1);
        teapotShader.setVec3("ambient_color", ambient_color);
        teapotShader.setVec3("specular_color", specular_color);
        teapotShader.setVec3("diffuse_color", diffuse_color);

        static float shininess{25};
        ImGui::SliderFloat("shininess", &shininess, 0, 250);
        teapotShader.setFloat("shininess", shininess);

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
        // if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        //     cout << "mouse sag buton release tetiklendi. " << endl;
        // }
        // if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        //     cout << "left one selammsss release tetiklendik. " << endl;
        // }
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
        static float fovY{45};

        if(yoffset == 1) { // zoom in.

            fovY -= (float)0.5f;
            if(fovY<1) fovY = 1.0f;
            glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
            teapotShaderPtr->useProgram();
            teapotShaderPtr->setMat4("projection", proj);
            planeShaderPtr->useProgram();
            planeShaderPtr->setMat4("projection", proj);

        } else if(yoffset == -1) { // zoom out.

            fovY += (float)0.5f;
            if(fovY>=88) fovY = 88.0f;
            glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
            teapotShaderPtr->useProgram();
            teapotShaderPtr->setMat4("projection", proj);
            planeShaderPtr->useProgram();
            planeShaderPtr->setMat4("projection", proj);

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
            rx = glm::rotate(rx, glm::radians(rotate_y_axis), glm::vec3(0, 1, 0));
            ry = glm::rotate(ry, glm::radians(rotate_x_axis), glm::vec3(1, 0, 0));

        }

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // right mouse button. adjust translation.

            // cout << "(x: "<< x <<  ", y: "<< y << ")" << endl;
            float translate_z{};
            translate_z = (float)diffY/30.0f;
            tz = glm::translate(tz, glm::vec3(0, 0, -translate_z));

        }

        transformation_in_camera_frame = tz * ry * rx;
        teapotShaderPtr->useProgram();
        teapotShaderPtr->setMat4("transformation_in_camera_frame", transformation_in_camera_frame);
        planeShaderPtr->useProgram();
        planeShaderPtr->setMat4("transformation_in_camera_frame", transformation_in_camera_frame);
        
        lastX = x; lastY = y;

    }
}