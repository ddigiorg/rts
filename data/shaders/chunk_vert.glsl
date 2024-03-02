#version 460 core

layout (location=0) in vec2 vPosVS;
layout (location=1) in vec2 vTexVS;

out vec2 vTexFS;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(vPosVS, 0.0, 1.0);
    vTexFS = vTexVS;
}