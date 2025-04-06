#version 450 core

out vec3 direction;

layout(location = 0) in vec3 a_pos;

uniform mat4 u_invView, u_invProj;


void main() {

    vec4 envPosInWorld = u_invView * u_invProj * vec4(a_pos, 1);
    direction = (envPosInWorld).xyz;
    gl_Position = vec4(a_pos, 1);

}