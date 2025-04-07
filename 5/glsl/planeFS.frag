#version 450 core

uniform sampler2D renderedTextureId;
uniform samplerCube environmentMap;
in vec2 f_texCoord;
// in vec3 f_pos, f_nrm;
in vec3 f_nrm;
in vec4 f_pos;
out vec4 fragColor;

vec4 calculateEnvironmentColor();
void main() {



    // fragColor = vec4(1, 1, 1, 1);
    // fragColor = texture(renderedTextureId, f_texCoord);
    // fragColor = vec4(1, 1, 1, 1);

    fragColor = calculateEnvironmentColor();
    
    // fragColor = vec4(0, 0, 0, 1);
}


vec4 calculateEnvironmentColor() {
    vec3 unit_normal = normalize(f_nrm);
    // vec3 unit_reflection_dir = normalize(f_pos);
    vec3 unit_reflection_dir = normalize(f_pos.xyz);
    float cosTheta = dot(unit_normal, -unit_reflection_dir);
    vec3 projectedVector = cosTheta * unit_normal;
    unit_reflection_dir = 2 * projectedVector + unit_reflection_dir;
    
    vec4 col = texture(environmentMap, unit_reflection_dir);

    return col;

}