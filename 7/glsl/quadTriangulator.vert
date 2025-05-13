#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;
// i dont prefer to send texture coords. 
// because tessellation coordinates are already can be used as texture coordinates.


out vec3 cont_nrm;
void main() {

    cont_nrm = a_nrm;
    gl_Position = vec4(a_pos, 1);

}