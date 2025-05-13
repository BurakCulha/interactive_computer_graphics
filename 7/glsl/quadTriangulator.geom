#version 450 core
layout(triangles) in;
layout(line_strip, max_vertices = 4) out;


void main() {

    // subtracting (0, 0, 0.1) just to make line segment closer to the camera.
    // this makes our line strip visible and makes sure about z fighting.
    gl_Position = gl_in[0].gl_Position - vec4(0, 0, 0.2, 0);
    EmitVertex();

    gl_Position = gl_in[1].gl_Position - vec4(0, 0, 0.2, 0);
    EmitVertex();

    gl_Position = gl_in[2].gl_Position - vec4(0, 0, 0.2, 0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position - vec4(0, 0, 0.2, 0);
    EmitVertex();

    EndPrimitive();
}