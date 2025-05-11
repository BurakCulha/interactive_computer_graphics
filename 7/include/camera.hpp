#ifndef MY_CAMERA_HPP
#define MY_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>


class Camera {
    private:
        glm::vec3 camPos;
        glm::vec3 camTarg;
        glm::vec3 camUp;
        glm::mat4 viewMatrix;
        glm::vec3 camX, camY, camZ;

        void initLookAt();

    public:
        Camera(glm::vec3 camPos = glm::vec3(0, 0, 10), glm::vec3 camTarg = glm::vec3(0, 0, 0), glm::vec3 camUp = glm::vec3(0, 1, 0));
        glm::mat4 getLookAtMatrix();
        glm::vec3 getCamPos();
        glm::vec3 getCamTarg();
        glm::vec3 getCamUp();
        glm::vec3 getCamX();
        glm::vec3 getCamY();
        glm::vec3 getCamZ();
        void changeProperties(glm::vec3 camPos = glm::vec3(0, 0, 10), glm::vec3 camTarg = glm::vec3(0, 0, 0), glm::vec3 camUp = glm::vec3(0, 1, 0));


};


#endif

