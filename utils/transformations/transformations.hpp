#ifndef MY_TRANSFORMATIONS_HPP
#define MY_TRANSFORMATIONS_HPP

#include <glm/glm.hpp>


glm::mat4 perspectiveTransformation(float near, float far);
glm::mat4 orthographicProjection(float near, float far, float top, float bottom, float right, float left);
glm::mat4 perspectiveProjection_constRLTB(float fovY, float aspectRatio, float far);
glm::mat4 perspectiveProjection_constNear(float fovY, float aspectRatio, float near, float far);

glm::mat4 reflectOverPlaneMatrix(float a, float b, float c, float d);
#endif