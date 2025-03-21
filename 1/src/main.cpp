#include <iostream>
#include <GL/freeglut.h>

float red = 0;
float green = 0;
float blue = 0;
int WINDOW_WIDTH = 750;
int WINDOW_HEIGHT = 600;
const int VIEWPORT_WIDTH = 500;
const int VIEWPORT_HEIGHT = 500;



void myDisplay();
void myKeyboard(unsigned char key, int x, int y);
void myPassiveMouse(int x, int y);
void myIdle();
void reshapeFunction(int width, int height);
int main(int argc, char** argv) {

    // glut initializations
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutCreateWindow("OpenGL Window 1");

    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutPassiveMotionFunc(myPassiveMouse);
    glutIdleFunc(myIdle);
    glutReshapeFunc(reshapeFunction);

    // opengl initializations
    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);



    glutMainLoop();


    return 0;
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutSwapBuffers();
    return;
}

void myKeyboard(unsigned char key, int x, int y) {
    // when esc is pressed, window closes.
    if(key == 27) {
        glutLeaveMainLoop();
    }
    return;
}

void myIdle() {
    // updates blue color by the time. 
    int ms = glutGet(GLUT_ELAPSED_TIME) % 1000;
    blue = (float)ms/1000; // [0, 1] range.
    glClearColor(red, green, blue, 1);

    glutPostRedisplay();
}

void myPassiveMouse(int x, int y) {
    // upper left corner is origin.
    // x increases to the right.
    // y increases to the bottom.
    // these coordinates are values of red and green.
    // red is bound to y value. 
    // green is bound to x value.
    red = ((float)y) / VIEWPORT_WIDTH;
    green = ((float)x) / VIEWPORT_WIDTH;

    return;
}

void reshapeFunction(int width, int height) {
    // keeps viewport size constant.
    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    glutPostRedisplay();
}