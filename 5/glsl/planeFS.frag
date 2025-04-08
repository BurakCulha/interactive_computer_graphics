#version 450 core

uniform sampler2D renderedTextureId;
uniform samplerCube environmentMap;
uniform vec2 textureResolution;

in vec3 f_nrm;
in vec4 f_pos;
out vec4 fragColor;

vec4 calculateEnvironmentColor();
vec4 calculateRenderedTextureColor();
void main() {

    vec4 textureColor = calculateRenderedTextureColor();
    vec4 environmentColor = calculateEnvironmentColor();

    if(textureColor.a == 0) {
        fragColor = environmentColor;
    } else {
        fragColor = textureColor;
    }
    
}

vec4 calculateRenderedTextureColor() {
    
    // this texture coord is like see through glass. 
    // through that glass obtain texture color. 
    // that texture is spread over [-1, 1) for both x, and y axes.
    // we get normalized texture coordinate with that.
    // an alternative would be using texture rect with only gl_fragcoord.
    vec2 textureCoord = gl_FragCoord.xy / textureResolution.xy;
    vec4 col = texture(renderedTextureId, textureCoord);
    return col;
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