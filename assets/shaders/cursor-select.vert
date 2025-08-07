#version 460 core

layout(location = 0) in vec2 aPosition;

out vec4 fColor;

uniform vec4 uColor;
uniform mat4 uVP; // view projection matrix

void main() {
    gl_Position = uVP * vec4(aPosition, 1.0, 1.0);
    fColor = uColor;
}