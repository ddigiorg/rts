#version 460 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aWorldPosition;
layout(location = 2) in vec2 aScale;

uniform mat4 uVP; // view projection matrix
uniform vec2 uOffset; // offset for drop shadow

void main() {
    vec2 pos = aWorldPosition + (aVertexPosition * aScale) + uOffset;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);
}