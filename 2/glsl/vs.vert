#version 450 core

layout(location = 0)in vec3 a_pos;
layout(location = 2)in vec2 a_texCoord;

uniform mat4 transformation_in_camera_frame;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec2 f_texCoord;



void main() {
    // gl_Position = vec4(0, 0, 0, 1);
    gl_Position = projection * transformation_in_camera_frame * view * model * vec4(a_pos, 1);
    f_texCoord = a_texCoord;
}