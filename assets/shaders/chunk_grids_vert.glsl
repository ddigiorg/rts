#version 460 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aPosition;
layout(location = 2) in vec2 aOffset;

out vec4 vColor;

uniform vec2 uVertexSize;
uniform vec4 uColor;
uniform mat4 uVP; // view projection matrix

void main() {

    // calculate vertex position
    vec2 pos = aVertexPosition * uVertexSize + aPosition + aOffset;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // pass variables to fragment shader
    vColor = uColor;
}