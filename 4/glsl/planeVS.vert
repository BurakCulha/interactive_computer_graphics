#version 450 core

layout (location = 0) in vec3 a_pos;
layout (location = 2) in vec2 a_texCoord;

out vec2 f_texCoord;

uniform mat4 model, view, projection, transformation_in_camera_frame;

void main() {
    mat4 mvp = projection * transformation_in_camera_frame * view * model;
    gl_Position = mvp * vec4(a_pos, 1);
    f_texCoord = a_texCoord;
}