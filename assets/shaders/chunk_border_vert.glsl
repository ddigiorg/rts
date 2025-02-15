#version 460 core

layout(location = 0) in vec2 aChunkVertexPosition;
layout(location = 1) in vec2 aChunkPosition;

out vec4 vColor;

uniform vec4 uColor;
uniform mat4 uVP; // view projection matrix

void main() {

    // calculate chunk vertex position
    vec2 pos = aChunkVertexPosition + aChunkPosition;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // pass variables to fragment shader
    vColor = uColor;
}