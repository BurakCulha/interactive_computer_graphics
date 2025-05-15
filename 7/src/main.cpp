#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include "transformations.hpp"
#include "shaderProgram.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "framebuffer.hpp"

using namespace std;

ShaderProgram *teapotShaderPtr, *shadowShaderPtr;
ShaderProgram *quadTriangulatorPtr, *quadDisplacerPtr, *quadShadowPtr, *cameraShaderPtr;

int lastX{}, lastY{};
float fovY_shadowLight{45.0f}, fovY_scene{45.0};

glm::mat4 rx(1), ry(1), tz(1), transformation_in_camera_frame(1);
// glm::mat4 rx_plane(1), ry_plane(1), tz_plane(1), transformation_in_camera_frame_plane(1);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main() {

    if(!glfwInit()) {
        cout << "glfw could not be initialized. " << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    GLFWwindow* windowPtr = glfwCreateWindow(600, 600, "render to texture", nullptr, nullptr);
    if(!windowPtr) {
        cout << "failed to create glfw window. " << endl;
        return -2;
    }
    glfwMakeContextCurrent(windowPtr);
    glfwSetFramebufferSizeCallback(windowPtr, framebuffer_size_callback);
    glfwSetScrollCallback(windowPtr, scroll_callback);

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


    ShaderProgram quadDisplacer("../glsl/quadDisplacer.vert", "../glsl/quadDisplacer.frag");
    quadDisplacer.createShader("../glsl/quadDisplacer.tesc", ShaderProgram::shaderTypes::tesControlShader);
    quadDisplacer.createShader("../glsl/quadDisplacer.tese", ShaderProgram::shaderTypes::tesEvaluationShader);
    quadDisplacer.linkShaderObject();
    quadDisplacerPtr = &quadDisplacer;
    quadDisplacer.useProgram();

    // ShaderProgram quadTriangulator("../glsl/quadTriangulator.vert", "../glsl/quadTriangulator.frag");
    // quadTriangulator.createShader("../glsl/quadTriangulator.tesc", ShaderProgram::shaderTypes::tesControlShader);
    // quadTriangulator.createShader("../glsl/quadTriangulator.tese", ShaderProgram::shaderTypes::tesEvaluationShader);
    // quadTriangulator.createShader("../glsl/quadTriangulator.geom", ShaderProgram::shaderTypes::geometryShader);
    // quadTriangulator.linkShaderObject();
    // quadTriangulatorPtr = &quadTriangulator;

    
    // ShaderProgram quadShadow;

    Mesh quadMesh("../objFiles/quad.obj");
    quadMesh.myTextureLoader("../img/teapot_disp.png", "displacementMapTexture");
    quadMesh.myTextureLoader("../img/teapot_normal.png", "normalMapTexture.png");
    quadMesh.setPatchSize(4);

    // quadDisplacer.setMat4("mv", glm::mat4(1));
    // quadDisplacer.setMat4("mvp", glm::mat4(1));

    // vector<glm::vec3> poss = quadMesh.getUniqueVertexPositions();
    // for(int i=0; i<poss.size(); i++) {
    //     cout << i <<". vertex: " << glm::to_string(poss[i]) << endl;
    // }

    Camera camera(glm::vec3(0, 0.0, 1.4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 view = camera.getLookAtMatrix();
    glm::mat4 proj = perspectiveProjection_constNear(fovY_scene, 1.0f, -1.0f, -1000.0f);
    quadDisplacer.setMat4("mv", view);
    quadDisplacer.setMat4("mvp", proj * view);
    quadDisplacer.setVec3("light_target", glm::vec3(view * glm::vec4(0, 0, 0, 1)));
    quadDisplacer.setVec3("diffuse_color", glm::vec3(0.25, 0.25, 0.25));



    ShaderProgram quadTriangulator("../glsl/quadTriangulator.vert", "../glsl/quadTriangulator.frag");
    quadTriangulator.createShader("../glsl/quadTriangulator.tesc", ShaderProgram::shaderTypes::tesControlShader);
    quadTriangulator.createShader("../glsl/quadTriangulator.tese", ShaderProgram::shaderTypes::tesEvaluationShader);
    quadTriangulator.createShader("../glsl/quadTriangulator.geom", ShaderProgram::shaderTypes::geometryShader);
    quadTriangulator.linkShaderObject();
    quadTriangulatorPtr = &quadTriangulator;
    quadTriangulator.useProgram();
    // quadTriangulator.setMat4("mv", view);
    quadTriangulator.setMat4("mvp", proj * view);



    ShaderProgram quadShadow("../glsl/quadShadow.vert", "../glsl/quadShadow.frag");
    quadShadow.createShader("../glsl/quadShadow.tese", ShaderProgram::shaderTypes::tesEvaluationShader);
    quadShadow.createShader("../glsl/quadShadow.tesc", ShaderProgram::shaderTypes::tesControlShader);
    quadShadow.linkShaderObject();
    quadShadow.useProgram();
    quadShadowPtr = &quadShadow;
    glm::mat4 quadShadowProjectionMatrix = perspectiveProjection_constNear(fovY_shadowLight, 1, -1, -1000);
    // quadShadow.setMat4("mlp", glm::mat4(1));




    ShaderProgram cameraShader("../glsl/cameraShader.vert", "../glsl/cameraShader.frag");
    cameraShader.linkShaderObject();
    cameraShader.useProgram();
    cameraShaderPtr = &cameraShader;
    // cameraShader.setMat4("mvp", glm::mat4(1));

    Mesh shadowLightMesh("../objFiles/light.obj");
    shadowLightMesh.transformMesh(glm::scale(glm::mat4(1), glm::vec3(0.51)));
    glm::vec3 shadowLightPosition(1), shadowLightTarget(0, 0, 0), shadowLightUp(0, 1, 0);
    Camera shadowLightCamera(shadowLightPosition, shadowLightTarget, shadowLightUp);
    glm::mat4 shadowLightMeshOrienterMatrix = glm::inverse(shadowLightCamera.getLookAtMatrix());

    cameraShader.setMat4("mvp", proj * view * shadowLightMeshOrienterMatrix * shadowLightMesh.getModelMatrix());



    Framebuffer fb(600, 600);
    fb.createDepthTexture("shadowMap");
    fb.configureFramebufferForDepthMap();

    quadDisplacer.useProgram();
    quadDisplacer.setInt(fb.getTexture().type.c_str(), fb.getTexture().textureUnit);
    glm::mat4 shadowMatrix = glm::translate(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5)) * glm::scale(glm::mat4(1), glm::vec3(0.5)) * quadShadowProjectionMatrix * shadowLightCamera.getLookAtMatrix();
    quadDisplacer.setMat4("shadowMatrix", shadowMatrix);


    quadShadow.useProgram();
    quadShadow.setMat4("mlp", quadShadowProjectionMatrix * shadowLightCamera.getLookAtMatrix());

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
    // ShaderProgram teapotShader("../glsl/teapotVS.vert", "../glsl/teapotFS.frag");
    // teapotShader.useProgram();
    // teapotShaderPtr = &teapotShader;

    // // Mesh teapotMesh("../objFiles/sphere.obj");
    // Mesh teapotMesh("../objFiles/teapot.obj");
    
    // Camera camera(glm::vec3(0, 2.5, 5.4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    // glm::mat4 view = camera.getLookAtMatrix();
    // glm::mat4 proj = perspectiveProjection_constNear(fovY_scene, 1.0f, -1.0f, -1000.0f);

    // teapotShader.setMat4("projection", proj);
    // teapotShader.setMat4("view", view);
    // teapotShader.setMat4("model", teapotMesh.getModelMatrix());
    // teapotShader.setMat4("transformation_in_camera_frame", transformation_in_camera_frame);

    // Mesh planeMesh("../objFiles/plane_2.obj");
    // planeMesh.transformMesh(glm::rotate(glm::mat4(1), glm::radians(-90.0f), glm::vec3(1, 0, 0)));
    // planeMesh.transformMesh(glm::scale(glm::mat4(1), glm::vec3(2.0f)));

    // //////////////////////////////////////////

    // ShaderProgram shadowShader("../glsl/shadow.vert", "../glsl/shadow.frag");
    // shadowShader.useProgram();
    // shadowShaderPtr = &shadowShader;
    // Camera shadowLight(glm::vec3(0, 1.75, -2.4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    // shadowShader.setMat4("model", teapotMesh.getModelMatrix());
    // shadowShader.setMat4("light", shadowLight.getLookAtMatrix());
    // glm::mat4 shadowProjMatrix = perspectiveProjection_constNear(fovY_shadowLight, 1, -1, -1000);
    // shadowShader.setMat4("projection", shadowProjMatrix);



    // Framebuffer fb(600, 600);

    // fb.createDepthTexture("shadowMap");
    // fb.configureFramebufferForDepthMap();


    // teapotShader.useProgram();
    // teapotShader.setInt(fb.getTexture().type, fb.getTexture().textureUnit);
    // glm::mat4 shadowMatrix = glm::translate(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5 - 0.001284)) * glm::scale(glm::mat4(1), glm::vec3(0.5)) * proj * shadowLight.getLookAtMatrix() * teapotMesh.getModelMatrix();
    // teapotShader.setMat4("shadowMatrix", shadowMatrix);

    // glm::vec3 light_target, light_pos;
    // light_target = shadowLight.getCamTarg();
    // light_pos = shadowLight.getCamPos();

    // glm::vec4 res;
    // res = transformation_in_camera_frame * view * glm::vec4(light_pos, 1);
    // light_pos = glm::vec3(res);
    
    // res = transformation_in_camera_frame * view * glm::vec4(light_target, 1);
    // light_target = glm::vec3(res);

    // teapotShader.setVec3("light_target", light_target);
    // teapotShader.setVec3("light_position", light_pos);


    // Mesh lightMesh("../objFiles/light.obj");
    // glm::mat4 lightOrienterMatrix(1); // this matrix is used to locate and orient the light model.
    // lightOrienterMatrix = shadowLight.getLookAtMatrix();
    // lightOrienterMatrix = glm::inverse(lightOrienterMatrix);




    bool showTriangulation{false};

    while(!glfwWindowShouldClose(windowPtr)) {



        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        // fb.setAsRenderTarget();
        // glClearColor(0, 0, 0, 0); // shadowMap background color.
        // glClear(GL_DEPTH_BUFFER_BIT);
        
        // // // drawing should be here.

        // shadowShader.useProgram();

        // // teapotMesh.draw(shadowShader);

        // // no need to generate mipmap. but generating for suppressing renderdoc warning.
        // glGenerateTextureMipmap(fb.getTexture().id); // generate mipmap for rendered texture.

        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // glViewport(0, 0, 600, 600);
        // glClearColor(0, 0, 0, 1); // plane background color;
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // teapotShader.useProgram();
        // teapotShader.setMat4("model", teapotMesh.getModelMatrix());
        // teapotShader.setVec3("diffuse_color", glm::vec3(1, 0, 0));
        // // teapotMesh.draw(teapotShader);

        // teapotShader.setMat4("model", planeMesh.getModelMatrix());
        // teapotShader.setVec3("diffuse_color", glm::vec3(0, 1, 0));
        // // planeMesh.draw(teapotShader);


        // teapotShader.setMat4("model", lightOrienterMatrix);
        // teapotShader.setVec3("diffuse_color", glm::vec3(0, 0, 3));
        // // lightMesh.draw(teapotShader);
       


        ///////////

        // burda quad shadow rendering yapilir.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fb.setAsRenderTarget();
        glClearColor(0, 0, 0, 0); // shadowMap background color.
        glClear(GL_DEPTH_BUFFER_BIT);
        quadShadow.useProgram();
        quadMesh.drawPatches(quadShadow);

        glGenerateTextureMipmap(fb.getTexture().id); // generate mipmap for rendered texture.

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, 600, 600);
        glClearColor(0, 0, 0, 1); // plane background color;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        quadDisplacer.useProgram();
        // quadMesh.draw(quadDisplacer);
        quadMesh.drawPatches(quadDisplacer);

        if(showTriangulation) {
            quadTriangulator.useProgram();
            quadMesh.drawPatches(quadTriangulator);
        }

        cameraShader.useProgram();
        shadowLightMesh.draw(cameraShader);






        ///////////////////////// imgui things begin. ////////////////////////
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("imgui window for input for shading project");


        static glm::vec3 specular_color(1);

        ImGui::SliderFloat3("specular color: ", &specular_color.x, 0, 1);
        quadDisplacer.useProgram();
        quadDisplacer.setVec3("specular_color", specular_color);
        // teapotShader.setVec3("specular_color", specular_color);


        // static glm::vec3 lightObjPosition = shadowLight.getCamPos();
        static glm::vec3 lightObjPosition(1);
        static float sphericalAlpha{}, sphericalBeta{}, lightDistance{1};
        ImGui::SliderFloat("light spherical Alpha", &sphericalAlpha, -80, 80);
        ImGui::SliderFloat("light spherical Beta", &sphericalBeta, -78, 78);
        ImGui::SliderFloat("light object distance", &lightDistance, 1, 3.2);
        // lightObjPosition.y = 4 * glm::sin(glm::radians(sphericalAlpha));
        // lightObjPosition.z = 4 * glm::cos(glm::radians(sphericalAlpha)) * glm::cos(glm::radians(sphericalBeta));
        // lightObjPosition.x = 4 * glm::cos(glm::radians(sphericalAlpha)) * glm::sin(glm::radians(sphericalBeta));
        lightObjPosition.y = lightDistance * glm::sin(glm::radians(sphericalAlpha));
        lightObjPosition.z = lightDistance * glm::cos(glm::radians(sphericalAlpha)) * glm::cos(glm::radians(sphericalBeta));
        lightObjPosition.x = lightDistance * glm::cos(glm::radians(sphericalAlpha)) * glm::sin(glm::radians(sphericalBeta));
        shadowLightPosition = lightObjPosition;

        shadowLightCamera.changeProperties(shadowLightPosition, shadowLightTarget, shadowLightUp);
        shadowLightMeshOrienterMatrix = glm::inverse(shadowLightCamera.getLookAtMatrix());
 
        // shadowLight.changeProperties(lightObjPosition, shadowLight.getCamTarg(), shadowLight.getCamUp());
        // lightOrienterMatrix = glm::inverse(shadowLight.getLookAtMatrix());
        // ImGui::SliderFloat3("lightpos:", &lightObjPosition.x, -2, 2);

        static float shininess{25};
        ImGui::SliderFloat("shininess", &shininess, 0, 250);
        quadDisplacer.setFloat("shininess", shininess);
        // teapotShader.setFloat("shininess", shininess);
        // glm::vec4 lightObjPositionInViewSpace = transformation_in_camera_frame * view * glm::vec4(lightObjPosition, 1);
        glm::vec4 lightObjPositionInViewSpace = view * glm::vec4(lightObjPosition, 1);
        // teapotShader.setVec3("light_position", glm::vec3(lightObjPositionInViewSpace));


        static float bias{};
        ImGui::SliderFloat("bias", &bias, -1.8, 1.8);
        // static float light_angle_property{45};
        static float light_angle_property{45};
        ImGui::SliderFloat("light angle property", &light_angle_property, 0, 88.0f);
        // teapotShader.setFloat("light_angle_property", glm::radians(light_angle_property));
        fovY_shadowLight = light_angle_property;
        quadShadowProjectionMatrix = perspectiveProjection_constNear(fovY_shadowLight, 1, -1, -1000);

        // shadowMatrix = glm::translate(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5 + bias/100.0f)) * glm::scale(glm::mat4(1), glm::vec3(0.5)) * shadowProjMatrix * shadowLight.getLookAtMatrix();
        // teapotShader.setMat4("shadowMatrix", shadowMatrix);

        // shadowShader.useProgram();
        // shadowShader.setMat4("light", shadowLight.getLookAtMatrix());
        // shadowProjMatrix = perspectiveProjection_constNear(fovY_shadowLight, 1, -1, -1000);
        // shadowShader.setMat4("projection", shadowProjMatrix);


        static glm::ivec4 outerLevels{1}, innerLevels{1};
        static float u_exaggerationFactor{};
        ImGui::SliderInt4("outer levels: ", &outerLevels.x, 1, 100);
        ImGui::SliderInt2("inner levels: ", &innerLevels.x, 1, 100);
        ImGui::SliderFloat("exaggeration factor: ", &u_exaggerationFactor, 0.001, 0.4);
        ImGui::Checkbox("show triangulation", &showTriangulation);
        
        quadDisplacer.useProgram();
        quadDisplacer.setVec4int("OL", outerLevels);
        quadDisplacer.setVec2int("IL", innerLevels);
        quadDisplacer.setFloat("u_exaggerationFactor", u_exaggerationFactor);
        quadDisplacer.setVec3("light_position", glm::vec3(lightObjPositionInViewSpace));
        quadDisplacer.setFloat("light_angle_property", glm::radians(light_angle_property));
        shadowMatrix = glm::translate(glm::mat4(1), glm::vec3(0.5, 0.5, 0.5 - bias/100.0f)) * glm::scale(glm::mat4(1), glm::vec3(0.5)) * quadShadowProjectionMatrix * shadowLightCamera.getLookAtMatrix(); 
        quadDisplacer.setMat4("shadowMatrix", shadowMatrix);

        quadTriangulator.useProgram();
        quadTriangulator.setVec4int("OL", outerLevels);
        quadTriangulator.setVec2int("IL", innerLevels);
        quadTriangulator.setFloat("u_exaggerationFactor", u_exaggerationFactor);

        quadShadow.useProgram();
        quadShadow.setVec4int("OL", outerLevels);
        quadShadow.setVec2int("IL", innerLevels);
        quadShadow.setFloat("u_exaggerationFactor", u_exaggerationFactor);
        quadShadow.setMat4("mlp", quadShadowProjectionMatrix * shadowLightCamera.getLookAtMatrix());


        cameraShader.useProgram();
        cameraShader.setMat4("mvp", proj * view * shadowLightMeshOrienterMatrix * shadowLightMesh.getModelMatrix());


        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ///////////////////////// imgui things end. ////////////////////////

        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////

        glfwSwapBuffers(windowPtr);
        glfwPollEvents();
    }
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////




    //// imgui terminate
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw teriminate. 
    glfwDestroyWindow(windowPtr);
    glfwTerminate();

    return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // if(!ImGui::GetIO().WantCaptureMouse) {

    //     cout << "y offset: " << yoffset << endl;
    //     static float fovY{25};
    //     static float fovY_plane{25};

    //     if(yoffset == 1) { // zoom in.

            
    //         if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    //             fovY_plane -= 0.5f;
    //             if(fovY_plane<=1) fovY_plane = 1.0f;
    //             glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);


    //         } else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { // set both plane and teapot.
    //             fovY_plane -= 0.5f;
    //             if(fovY_plane<=1) fovY_plane = 1.0f;
    //             glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);


    //             fovY -= (float)0.5f;
    //             if(fovY<1) fovY = 1.0f;
    //             glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
    //             teapotShaderPtr->useProgram();
    //             teapotShaderPtr->setMat4("projection", proj);

    //         } else {
    //             fovY -= (float)0.5f;
    //             if(fovY<1) fovY = 1.0f;
    //             glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
    //             teapotShaderPtr->useProgram();
    //             teapotShaderPtr->setMat4("projection", proj);
    //         }

    //     } else if(yoffset == -1) { // zoom out.

    //         if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    //             fovY_plane += 0.5f;
    //             if(fovY_plane>=88) fovY_plane = 88.0f;
    //             glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);

    //         } else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { // set both plane and teapot.

    //             fovY += (float)0.5f;
    //             if(fovY>=88) fovY = 88.0f;
    //             glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
    //             teapotShaderPtr->useProgram();
    //             teapotShaderPtr->setMat4("projection", proj);

    //             fovY_plane += 0.5f;
    //             if(fovY_plane>=88) fovY_plane = 88.0f;
    //             glm::mat4 proj_plane = perspectiveProjection_constNear(fovY_plane, 1.0f, -1.0f, -1000.0f);


    //         } else {
    //             fovY += (float)0.5f;
    //             if(fovY>=88) fovY = 88.0f;
    //             glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
    //             teapotShaderPtr->useProgram();
    //             teapotShaderPtr->setMat4("projection", proj);
    //         }
    //     }
    // }
}
