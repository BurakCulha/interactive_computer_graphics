#version 450 core

uniform samplerCube environmentMap;
uniform sampler2D texture_diffuse;
uniform vec3 light_direction;
uniform vec3 ambient_color, diffuse_color, specular_color;
uniform float shininess;

in vec2 f_texCoord;
in vec3 f_nrm;
// in vec3 f_pos;
in vec4 f_pos;

out vec4 fragColor;

vec4 specularColorFunction();
vec4 diffuseColorFunction();
vec4 calculateEnvironmentColor();
void main() {
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);

    // vec3 f_nrm_unit = normalize(f_nrm);

    // fragColor = texture(texture_diffuse, f_texCoord);
    // fragColor = vec4(ambient_color * diffuse_color * specular_color, 1) * shininess;
    // fragColor = vec4(f_nrm_unit, 1) * vec4(light_direction, 1) + fragColor * 0.001;

    // fragColor = diffuseColorFunction() + fragColor * 0.0001 * vec4(specular_color * ambient_color, 1);
    // fragColor = specularColorFunction() + 0.00001 * fragColor;


    // fragColor = texture(texture_diffuse, f_texCoord) * (diffuseColorFunction() + specularColorFunction()) + 0.5 * vec4(ambient_color,0);

    // fragColor = fragColor * 0.001 + vec4(1, 1, 0, 1);

    // fragColor = vec4(0.73, 0.73, 0.73, 1.0) * calculateEnvironmentColor() + fragColor*0.00001;
    fragColor = vec4(0.73, 0.73, 0.73, 1.0) * calculateEnvironmentColor();

}

vec4 diffuseColorFunction() {
    vec4 res;
    vec4 K_d = vec4(diffuse_color, 1);

    vec3 light_direction_unit = normalize(light_direction);
    vec3 fragment_normal_unit = normalize(f_nrm);

    float cosTheta = dot(light_direction_unit, fragment_normal_unit);

    vec4 intensity = vec4(1, 1, 1, 1);

    res = intensity * max(cosTheta, 0) * K_d;
    return res;
}

vec4 specularColorFunction() {
    vec4 res;
    vec4 K_s = vec4(specular_color, 1);

    vec3 light_direction_unit = normalize(light_direction);
    vec3 fragment_normal_unit = normalize(f_nrm);
    // vec3 view_direction_unit = normalize(-f_pos);
    vec3 view_direction_unit = normalize(-f_pos.xyz);
    float cosTheta = dot(light_direction_unit, fragment_normal_unit);
    vec4 intensity = vec4(1, 1, 1, 1);
    vec3 half_vector_unit = normalize(light_direction_unit + view_direction_unit);
    float cosPhi = dot(half_vector_unit, fragment_normal_unit);
    cosPhi = max(cosPhi, 0);

    res = intensity * (1/cosTheta) * K_s * pow(cosPhi, shininess);

    return res;
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