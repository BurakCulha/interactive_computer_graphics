#include "transformations.hpp"

glm::mat4 perspectiveTransformation(float near, float far) {
    glm::mat4 pt(1);
    pt[0][0] = -near;
    pt[1][1] = -near;
    pt[2][2] = -(near + far);
    pt[2][3] = -1;
    pt[3][2] = near * far;
    pt[3][3] = 0;

    return pt;
}

glm::mat4 orthographicProjection(float near, float far, float top, float bottom, float right, float left) {

    glm::mat4 orthoProjection(1);

    // scaling parts.
    orthoProjection[0][0] = 2/(right-left);
    orthoProjection[1][1] = 2/(top-bottom);
    orthoProjection[2][2] = 2/(far - near); // flips z direction. far<near<0.
    // orthoProjection[2][2] = 2/(near - far); // not flips z direction. far<near<0.

    // translating parts.
    orthoProjection[3][0] = -2*left/(right-left) - 1; // leftT = -1, rightT = 1;
    orthoProjection[3][1] = -2*bottom/(top-bottom) - 1; // bottom = -1, top = 1;  
    orthoProjection[3][2] = -2*near/(far-near) - 1; // near = -1, far = 1;


    return orthoProjection;
}

glm::mat4 perspectiveProjection_constNear(float fovY, float aspectRatio, float near, float far) {
    glm::mat4 persTraMatrix = perspectiveTransformation(near, far);
    float r, l, t, b;

    // tan(fovY) = top/(-near);
    t = glm::tan(glm::radians(fovY)) * abs(near);
    b = -t;
    // r/t = aspectRatio;
    r = t*aspectRatio;
    l = -r;

    glm::mat4 orthoProjMatrix = orthographicProjection(near, far, t, b, r, l);

    return orthoProjMatrix * persTraMatrix;
}

glm::mat4 perspectiveProjection_constRLTB(float fovY, float aspectRatio, float far) {

    float t = 1, b = -1;
    // aspect_ratio = right/top;
    float r, l;
    r = t * aspectRatio;
    l = -r;

    // tan(fovY) = top/(-near);
    float near = -1/glm::tan(glm::radians(fovY));
    
    glm::mat4 persTraMatrix = perspectiveTransformation(near, far);
    glm::mat4 orthoProjMatrix = orthographicProjection(near, far, t, b, r, l);

    return orthoProjMatrix * persTraMatrix;
}




glm::mat4 reflectOverPlaneMatrix(float a, float b, float c, float d) {
/**
 * aciklama yazacagim. buraya wikipediadan yazacagim.
 */

    glm::vec4 col1(1 - (2 * a * a), -2 * a * b, -2 * a *c, 0);
    glm::vec4 col2(-2 * a * b, 1 - (2 * b * b), -2 * b *c, 0);
    glm::vec4 col3(-2 * a *c, -2 * b * c, 1 - (2 * c * c), 0);
    glm::vec4 col4(-2 * a * d, -2 * b * d, -2 * c * d, 1);

    glm::mat4 result(col1, col2, col3, col4);
    return result;
}