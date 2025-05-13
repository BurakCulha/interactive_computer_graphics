#version 450 core

layout(vertices = 4) out;
uniform ivec4 OL;
uniform ivec2 IL;

in vec3 cont_nrm[];
out vec3 eval_nrm[];

void main() {
    gl_TessLevelOuter[0] = OL.x;
    gl_TessLevelOuter[1] = OL.y;
    gl_TessLevelOuter[2] = OL.z;
    gl_TessLevelOuter[3] = OL.w;
    gl_TessLevelInner[0] = IL.x;
    gl_TessLevelInner[1] = IL.y;

    eval_nrm[gl_InvocationID] = cont_nrm[gl_InvocationID];
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}