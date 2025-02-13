#version 410 core

vec2 verts[6] = vec2[](
    vec2(-1, -1),
    vec2(-1,  1),
    vec2( 1,  1),

    vec2(-1, -1),
    vec2( 1, -1),
    vec2( 1,  1)
);

vec2 uvs[6] = vec2[](
    vec2( 0,  1),
    vec2( 0,  0),
    vec2( 1,  0),

    vec2( 0,  1),
    vec2( 1,  1),
    vec2( 1,  0)
);

out vec2 UV;

uniform vec2 windowSize;
uniform vec2 imageSize;

uniform vec2 position;
uniform float scale;
uniform float aspect;
uniform float angle;

vec2 rotate(vec2 v, float degs) {
    float rads = radians(degs); // Convert degrees to radians
    float c = cos(rads);
    float s = sin(rads);
    
    return vec2(
        v.x * c - v.y * s,
        v.x * s + v.y * c
    );
}

void main() {
    vec2 scratchCSVertexPos = rotate(verts[gl_VertexID], 90 - angle) * imageSize * scale * 0.5;
    scratchCSVertexPos += position * (windowSize / vec2(480, 360));

    vec2 normalizedCoords = scratchCSVertexPos/(windowSize*0.5);

    gl_Position = vec4(normalizedCoords, 0, 1);
    UV = uvs[gl_VertexID];
}