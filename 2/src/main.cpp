#include <GL/glew.h>
#include "shaderProgram.hpp"
#include "transformations.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include <GL/freeglut.h>



// other includes above
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

ShaderProgram* shaderPtr;
Mesh* meshPtr;

int g_button{}, g_state{};
int lastX{}, lastY{};

glm::mat4 rx(1), ry(1), tz(1), transformation_in_camera_frame(1);


void renderF();
void idleF();
void reshapeF(int width, int height);
void mouseMotionF(int x, int y);
void mouseF(int button, int state, int x, int y);
int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitContextVersion(4, 5);
    glutInitContextFlags(GLUT_DEBUG);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowPosition(200, 200);
    glutInitWindowSize(600, 600);
    int windowId = glutCreateWindow("transformations");

    glutDisplayFunc(renderF);
    glutIdleFunc(idleF);
    // glutReshapeFunc(reshapeF);
    glutMotionFunc(mouseMotionF);
    glutMouseFunc(mouseF);

    if(glewInit() == GLEW_OK) {
        cout << "glew initialized successfully. " << endl;
    } else {
        cout << "glew could not be initialized. " << endl;
        cout << "GLEW--->INITIALIZATION::ERROR " << endl;
        cout << "exiting... " << endl;
        exit(-1);
    }

    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    ShaderProgram program("../glsl/vs.vert", "../glsl/fs.frag");
    program.useProgram();
    shaderPtr = &program;

    // Mesh me = Mesh("../objFiles/plane_2.obj");
    Mesh me = Mesh("../objFiles/teapot.obj");
    meshPtr = &me;
    me.myTextureLoader("../images/brick.png", "texture_diffuse");

    Camera camera(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 view = camera.getLookAtMatrix();
    glm::mat4 proj = perspectiveProjection_constNear(45.0f, 1.0f, -1.0f, -1000.0f);

    program.setMat4("projection", proj);
    program.setMat4("view", view);
    // program.setMat4("model", glm::mat4(1));

    me.centerTheMesh();
    program.setMat4("model", me.getModelMatrix());
    
    shaderPtr->setMat4("transformation_in_camera_frame", transformation_in_camera_frame);


    glutMainLoop();
    return 0;
}

void renderF() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDrawArrays(GL_POINT, 0, 1);

    meshPtr->draw(*shaderPtr);


    glutSwapBuffers();

}

void idleF() {

    glutPostRedisplay();
}

void reshapeF(int width, int height) {

}

void mouseMotionF(int x, int y) {
    int diffX = x - lastX;
    int diffY = y - lastY;


    if(g_button == 0) { // left mouse button. adjust rotation.
        float rotate_y_axis{}, rotate_x_axis{};
        rotate_y_axis = (float)diffX/10.0f;
        rotate_x_axis = (float)diffY/10.0f;
        rx = glm::rotate(rx, glm::radians(rotate_y_axis), glm::vec3(0, 1, 0));
        ry = glm::rotate(ry, glm::radians(rotate_x_axis), glm::vec3(1, 0, 0));

    } else if(g_button == 2) { // right mouse button. adjust translation.
        float translate_z{};
        translate_z = (float)diffY/30.0f;
        tz = glm::translate(tz, glm::vec3(0, 0, -translate_z));
    }

    transformation_in_camera_frame = tz * ry * rx;
    shaderPtr->setMat4("transformation_in_camera_frame", transformation_in_camera_frame);
    
    lastX = x; lastY = y;
}

void mouseF(int button, int state, int x, int y) {

    cout << "button: " << button << endl;
    cout << "state: " << state << endl;
    cout << "x: " << x << endl;
    cout << "y: " << y << endl;

    g_button = button;
    g_state = state;
    lastX = x; lastY = y;

    static float fovY = 45.0f;
    cout << "fovy: " << fovY << endl;
    if(g_button == 3) { // zoom in.
        fovY -= (float)0.5f;
        if(fovY<1) fovY = 1.0f;
        glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
        shaderPtr->setMat4("projection", proj);
    } else if(g_button == 4) { // zoom out.
        fovY += (float)0.5f;
        if(fovY>88) fovY = 88.0f;
        glm::mat4 proj = perspectiveProjection_constNear(fovY, 1.0f, -1.0f, -1000.0f);
        shaderPtr->setMat4("projection", proj);
    }
}