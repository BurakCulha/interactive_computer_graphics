#include "camera.hpp"


glm::mat4 Camera::getLookAtMatrix() {
    return this->viewMatrix;
}

Camera::Camera(glm::vec3 camPos, glm::vec3 camTarg, glm::vec3 camUp) {

    using namespace std;
    this->camPos = camPos;
    this->camTarg = camTarg;
    this->camUp = camUp;

    // cout << "pos:" << glm::to_string(this->camPos) << endl;
    // cout << "targ:" << glm::to_string(this->camTarg) << endl;
    // cout << "up:" << glm::to_string(this->camUp) << endl;

    this->initLookAt();
}

void Camera::initLookAt() {
    glm::vec3 normalizedZ = glm::normalize(this->camPos - this->camTarg);
    glm::vec3 normalizedY = this->camUp;
    glm::vec3 normalizedX = glm::normalize(glm::cross(normalizedY, normalizedZ));
    normalizedY = glm::normalize(glm::cross(normalizedZ, normalizedX));

    this->camX = normalizedX;
    this->camY = normalizedY;
    this->camZ = normalizedZ;

    
    // pos_in_world = (T * R) * pos_in_cam;   
    // (T*R) is a change of basis matrix from camera frame to world frame.
    // so,
    // (inverse(T*R)) * pos_in_world = pos_in_camera;
    // we now know how to transform a world position to a camera frame position.

    glm::mat4 rotationMatrix(glm::vec4(camX, 0),
                             glm::vec4(camY, 0),
                             glm::vec4(camZ, 0),
                             glm::vec4(0, 0, 0, 1));
    glm::mat4 rot_inv = glm::transpose(rotationMatrix);
    
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1), this->camPos);
    glm::mat4 transla_inv = glm::translate(glm::mat4(1), -this->camPos);

    this->viewMatrix = rot_inv * transla_inv;

    // using namespace std;
    // cout << "view matrix: " << glm::to_string(this->viewMatrix) << endl;
}

glm::vec3 Camera::getCamPos() {
    return this->camPos;
}

glm::vec3 Camera::getCamTarg() {
    return this->camTarg;
}

glm::vec3 Camera::getCamUp() {
    return this->camUp;
}

glm::vec3 Camera::getCamX() {
    return this->camX;
}

glm::vec3 Camera::getCamY() {
    return this->camY;
}

glm::vec3 Camera::getCamZ() {
    return this->camZ;
}

void Camera::changeProperties(glm::vec3 camPos, glm::vec3 camTarg, glm::vec3 camUp) {
    this->camPos = camPos;
    this->camTarg = camTarg;
    this->camUp = camUp;
    this->initLookAt();
}


