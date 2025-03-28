#version 450 core

in vec3 pos;

uniform mat4 model, view, projection, transformation_in_camera_frame;

void main() {
    mat4 mvp = projection * transformation_in_camera_frame * view * model;
    gl_Position = mvp * vec4(pos, 1);
}