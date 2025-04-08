#version 450 core


uniform samplerCube environmentMap;
in vec3 direction;
out vec4 fragColor;

void main() {

    fragColor = texture(environmentMap, direction);

}