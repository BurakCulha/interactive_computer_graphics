#version 450 core

uniform sampler2DShadow shadowMap;
uniform vec3 light_target, light_position; 
uniform float light_angle_property;
uniform vec3 ambient_color, diffuse_color, specular_color;
uniform float shininess;

in vec2 f_texCoord;
in vec3 f_nrm;
in vec4 f_pos;
in vec4 f_shadowMapPosition;

out vec4 fragColor;

vec4 specularColorFunction();
vec4 diffuseColorFunction();
vec4 calculateShadow();
vec4 blinnMaterialModel();
void main() {
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);

    // vec3 f_nrm_unit = normalize(f_nrm);

    // fragColor = texture(texture_diffuse, f_texCoord);
    // fragColor = vec4(ambient_color * diffuse_color * specular_color, 1) * shininess;
    // fragColor = vec4(f_nrm_unit, 1) * vec4(light_target, 1) + fragColor * 0.001;

    // fragColor = diffuseColorFunction() + fragColor * 0.0001 * vec4(specular_color * ambient_color, 1);
    // fragColor = specularColorFunction() + 0.00001 * fragColor;


    // fragColor = texture(texture_diffuse, f_texCoord) * (diffuseColorFunction() + specularColorFunction()) + 0.5 * vec4(ambient_color,0);

    // fragColor = fragColor * 0.001 + vec4(1, 1, 0, 1);

    // fragColor = vec4(0.73, 0.73, 0.73, 1.0) * calculateEnvironmentColor() + fragColor*0.00001;
    // fragColor = vec4(0.73, 0.73, 0.73, 1.0) * calculateEnvironmentColor();
    // fragColor = calculateShadow();
    // fragColor = vec4(diffuse_color, 1);
    // fragColor = calculateShadow() * 0.0001 + diffuseColorFunction();
    // fragColor = vec4(diffuse_color, 1) + fragColor * 0.0001;
    // fragColor = (specularColorFunction() + diffuseColorFunction()) + calculateShadow();
    // fragColor = (specularColorFunction() * calculateShadow()) + diffuseColorFunction();
    // fragColor = vec4(f_nrm, 1) + fragColor * 0.0001;
    // fragColor = blinnMaterialModel() * calculateShadow() + vec4(diffuse_color, 1) * 0.25;
    // vec4 useless = calculateShadow();
    // useless *= 0.00001;
    // fragColor = blinnMaterialModel() + useless + vec4(diffuse_color, 1) * 0.3;
    // fragColor = blinnMaterialModel() * calculateShadow() + vec4(diffuse_color, 1) * 0.00005;
    // fragColor *= 0.00001;
    // fragColor += diffuseColorFunction();
    // fragColor += specularColorFunction();
    // fragColor = blinnMaterialModel() * calculateShadow() + vec4(ambient_color, 1);
    fragColor = blinnMaterialModel() * calculateShadow() + vec4(ambient_color * 0.00001 + diffuse_color * 0.3, 1);

}

vec4 diffuseColorFunction() {

    vec3 fragment_normal_unit = normalize(f_nrm);
    vec3 light_target_vector_unit = normalize(light_target - light_position);
    vec3 light_to_fragment_dir = normalize(f_pos.xyz - light_position);
    float coslightingAngle = dot(light_target_vector_unit, light_to_fragment_dir);
    // coslightingAngle = max(0, coslightingAngle);
    float lightingAngle = acos(coslightingAngle);
    vec4 light_intensity = vec4(1, 1, 1, 1);

    vec4 res = vec4(0, 0, 0, 1);

    if(lightingAngle <= light_angle_property && lightingAngle >= -light_angle_property) {
        vec3 incoming_light_dir = -light_to_fragment_dir;
        float cosTheta = dot(incoming_light_dir, fragment_normal_unit);
        cosTheta = max(0, cosTheta);
        res = light_intensity * cosTheta * vec4(diffuse_color, 1);
    }

    return res;
}

vec4 specularColorFunction() {


    vec4 res = vec4(0, 0, 0, 1);
    vec4 K_s = vec4(specular_color, 1);
    vec3 fragment_normal_unit = normalize(f_nrm);
    vec3 light_target_vector_unit = normalize(light_target - light_position);
    vec3 light_to_fragment_dir = normalize(f_pos.xyz - light_position);
    float coslightingAngle = dot(light_target_vector_unit, light_to_fragment_dir);
    // coslightingAngle = max(0, coslightingAngle);
    float lightingAngle = acos(coslightingAngle);
    vec4 light_intensity = vec4(1, 1, 1, 1);

// if(lightingAngle <= light_angle_property && lightingAngle >=light_angle_property)

    // if(lightingAngle <= light_angle_property && lightingAngle >= -light_angle_property) {
    // if(lightingAngle <= light_angle_property && lightingAngle >= 0) {
    if(lightingAngle <= light_angle_property) {
        // vec3 view_direction_unit = normalize(-f_pos.xyz/f_pos.w);
        vec3 view_direction_unit = normalize(-f_pos.xyz);
        vec3 incoming_light_dir = normalize(-light_to_fragment_dir);
        vec3 halfVector = normalize(view_direction_unit + incoming_light_dir);

        float cosTheta = dot(incoming_light_dir, fragment_normal_unit);
        cosTheta = max(0, cosTheta);

        float cosPhi = dot(halfVector, fragment_normal_unit);
        cosPhi = max(0, cosPhi);
        vec4 dif_color = diffuseColorFunction();
        // res = light_intensity * cosTheta *  K_s * pow(cosPhi, shininess);
        res = light_intensity * K_s * pow(cosPhi, shininess);
    }

    return res;
}


vec4 calculateShadow() {
    vec4 res;
    // res = textureProj(shadowMap, f_pos);
    float decision = textureProj(shadowMap, f_shadowMapPosition);
    // if(decision == 0) {
    //     res = vec4(1, 1, 1, 1); 
    // } else {
    //     res = vec4(1, 0, 0, 1); 
    res = decision * vec4(1, 1, 1, 1);
    return res;
}

vec4 blinnMaterialModel() {
    vec4 res;
    // res = diffuseColorFunction() + specularColorFunction() + vec4(ambient_color, 1);
    res = diffuseColorFunction() + specularColorFunction();
    return res;
}