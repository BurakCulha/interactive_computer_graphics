#version 450 core

uniform sampler2D renderedTextureId;
in vec2 f_texCoord;
out vec4 fragColor;

void main() {


    // fragColor = vec4(1, 1, 1, 1);
    fragColor = texture(renderedTextureId, f_texCoord);
    // fragColor = vec4(1, 1, 1, 1);
}