#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nrm;

out vec3 cont_pos;
out vec3 cont_nrm;

void main() {

    cont_pos = a_pos;
    cont_nrm = a_nrm;
    gl_Position = vec4(a_pos, 1);
}