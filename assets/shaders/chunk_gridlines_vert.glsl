#version 460 core

layout(location = 0) in vec2 aTileVertexPosition;
layout(location = 1) in vec2 aTileOffset;
layout(location = 2) in vec2 aChunkOffset;

out vec4 vColor;

uniform vec4 uColor;
uniform mat4 uVP; // view projection matrix

void main() {

    // calculate tile vertex position
    vec2 pos = aTileVertexPosition + aTileOffset + aChunkOffset;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // pass variables to fragment shader
    vColor = uColor;
}