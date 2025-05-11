#version 450 core

uniform sampler2D normalMapTexture;
uniform vec3 light_target, light_position;
uniform float light_angle_property;

in vec4 frag_pos;
in vec2 frag_tex;


out vec4 fragColor;

void main() {

    fragColor = texture(normalMapTexture, frag_tex);
}