#version 450 core

uniform sampler2D normalMapTexture;
uniform sampler2DShadow shadowMap;
uniform vec3 light_target, light_position;
uniform float light_angle_property;
uniform vec3 diffuse_color, specular_color;
uniform float shininess;

in vec4 frag_pos;
in vec2 frag_tex;
in vec4 frag_shadowMapPosition;

out vec4 fragColor;

vec4 specularColorFunction();
vec4 diffuseColorFunction();
vec4 calculateShadow();
vec4 blinnMaterialModel();
void main() {

    // fragColor = texture(normalMapTexture, frag_tex) + frag_pos * 0.000001;
    vec4 ambient_color = vec4(diffuse_color * 0.3, 1);
    fragColor = blinnMaterialModel() * calculateShadow() + ambient_color;
    // fragColor = blinnMaterialModel() + calculateShadow() * 0.000001 + vec4(diffuse_color * 0.3, 1);
}

vec4 diffuseColorFunction() {

    vec3 f_nrm = (texture(normalMapTexture, frag_tex) * 2 ).xyz - 1;

    vec3 fragment_normal_unit = normalize(f_nrm);
    vec3 light_target_vector_unit = normalize(light_target - light_position);
    vec3 light_to_fragment_dir = normalize(frag_pos.xyz - light_position);
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

    vec3 f_nrm = (texture(normalMapTexture, frag_tex) * 2 ).xyz - 1;

    vec3 fragment_normal_unit = normalize(f_nrm);
    vec3 light_target_vector_unit = normalize(light_target - light_position);
    vec3 light_to_fragment_dir = normalize(frag_pos.xyz - light_position);
    float coslightingAngle = dot(light_target_vector_unit, light_to_fragment_dir);
    // coslightingAngle = max(0, coslightingAngle);
    float lightingAngle = acos(coslightingAngle);
    vec4 light_intensity = vec4(1, 1, 1, 1);

// if(lightingAngle <= light_angle_property && lightingAngle >=light_angle_property)

    if(lightingAngle <= light_angle_property && lightingAngle >= -light_angle_property) {
    // if(lightingAngle <= light_angle_property && lightingAngle >= 0) {
    // if(lightingAngle <= light_angle_property) {
        // vec3 view_direction_unit = normalize(-f_pos.xyz/f_pos.w);
        vec3 view_direction_unit = normalize(-frag_pos.xyz);
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
    float decision = textureProj(shadowMap, frag_shadowMapPosition);
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