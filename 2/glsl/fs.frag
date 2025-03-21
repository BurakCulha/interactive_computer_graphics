#version 450 core

uniform sampler2D texture_diffuse;
in vec2 f_texCoord;

out vec4 fragColor;
void main() {
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);

    fragColor = texture(texture_diffuse, f_texCoord);

}