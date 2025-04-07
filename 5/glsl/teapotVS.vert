#version 450 core

layout(location = 0)in vec3 a_pos;
layout(location = 2)in vec2 a_texCoord;
layout(location = 1)in vec3 a_nrm;

uniform mat4 transformation_in_camera_frame;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 reflection_matrix;

out vec2 f_texCoord;
out vec3 f_nrm;
// out vec3 f_pos;
out vec4 f_pos;



void main() {
    // gl_Position = vec4(0, 0, 0, 1);
    gl_Position = projection * transformation_in_camera_frame * view * reflection_matrix * model * vec4(a_pos, 1);
    f_texCoord = a_texCoord;
    f_nrm = (transpose(inverse(transformation_in_camera_frame * view * model)) * vec4(a_nrm,0)).xyz;
    // f_pos = (transformation_in_camera_frame * view * model * vec4(a_pos, 1)).xyz;
    f_pos = (transformation_in_camera_frame * view * model * vec4(a_pos, 1));
}