#version 450 core

uniform samplerCube environmentMap;


in vec3 f_nrm;
in vec4 f_pos;

out vec4 fragColor;


vec4 calculateEnvironmentColor();
void main() {
    
    fragColor = vec4(0.73, 0.73, 0.73, 1.0) * calculateEnvironmentColor();

}


vec4 calculateEnvironmentColor() {
    vec3 unit_normal = normalize(f_nrm);
    vec3 unit_reflection_dir = normalize(f_pos.xyz);
    float cosTheta = dot(unit_normal, -unit_reflection_dir);
    vec3 projectedVector = cosTheta * unit_normal;
    unit_reflection_dir = 2 * projectedVector + unit_reflection_dir;
    
    vec4 col = texture(environmentMap, unit_reflection_dir);

    return col;

}