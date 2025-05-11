#version 450 core

layout (location = 0)in vec3 a_pos;
uniform mat4 model, light, projection;

void main() {
    gl_Position = projection * light * model * vec4(a_pos, 1);
}