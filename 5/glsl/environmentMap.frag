#version 450 core


uniform samplerCube environmentMap;
in vec3 direction;
out vec4 fragColor;

void main() {

    fragColor = texture(environmentMap, direction);
    // fragColor = fragColor*0.00001 + vec4(direction, 1);
    

}