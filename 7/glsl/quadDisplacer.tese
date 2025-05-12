#version 450 core

layout(quads, equal_spacing, ccw) in;

uniform sampler2D displacementMapTexture;
uniform mat4 mv;
uniform mat4 mvp;
uniform float u_exaggerationFactor;

in vec3 eval_nrm[];
in vec2 eval_tex[];


out vec2 frag_tex;
out vec4 frag_pos;


vec4 interpolate_pos(vec4 p0, vec4 p1, vec4 p2, vec4 p3);
vec2 interpolate_tex(vec2 t0, vec2 t1, vec2 t2, vec2 t3);
vec3 interpolate_nrm(vec3 p0, vec3 p1, vec3 p2, vec3 p3);
vec4 evaluatePosition();
void main() {

    frag_tex = interpolate_tex(eval_tex[0], eval_tex[1], eval_tex[2], eval_tex[3]);
    vec4 eval_pos = evaluatePosition();
    frag_pos = mv * eval_pos;
    gl_Position = mvp * eval_pos;
    // gl_Position = interpolate_pos(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position) + 0.0001 * gl_Position;

}


vec4 interpolate_pos(vec4 p0, vec4 p1, vec4 p2, vec4 p3) {

    vec4 pos_x1, pos_x2, res;

    pos_x1 = mix(p0, p1, gl_TessCoord.x);
    // pos_x2 = mix(p3, p2, gl_TessCoord.x);
    pos_x2 = mix(p2, p3, gl_TessCoord.x);
    res = mix(pos_x1, pos_x2, gl_TessCoord.y);

    return res;
}

vec2 interpolate_tex(vec2 t0, vec2 t1, vec2 t2, vec2 t3) {
    vec2 tex1, tex2, res;

    tex1 = mix(t0, t1, gl_TessCoord.x);
    // tex2 = mix(t3, t2, gl_TessCoord.x);
    tex2 = mix(t2, t3, gl_TessCoord.x);
    res = mix(tex1, tex2, gl_TessCoord.y);

    return res;
}

vec3 interpolate_nrm(vec3 p0, vec3 p1, vec3 p2, vec3 p3) {

    vec3 n1, n2, res;

    n1 = mix(p0, p1, gl_TessCoord.x);
    // n2 = mix(p3, p2, gl_TessCoord.x);
    n2 = mix(p2, p3, gl_TessCoord.x);
    res = mix(n1, n2, gl_TessCoord.y);
    res = normalize(res);

    return res;
}

vec4 evaluatePosition() {
    vec4 evalPos;
    evalPos = interpolate_pos(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position);
    float heightAmount = texture(displacementMapTexture, frag_tex).r;
    heightAmount *= u_exaggerationFactor;
    vec3 nrm = interpolate_nrm(eval_nrm[0], eval_nrm[1], eval_nrm[2], eval_nrm[3]);
    vec4 dispAmount = vec4(nrm * heightAmount, 0);
    evalPos += dispAmount;

    return evalPos;
}