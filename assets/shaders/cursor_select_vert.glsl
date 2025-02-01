#version 460 core
layout(location = 0) in vec2 aPos;

uniform mat4 uVP; // view projection matrix

void main() {
    gl_Position = uVP * vec4(aPos, 0.0, 1.0);
}