#version 460 core
layout(location = 0) in vec2 aPosition; // tile vertex position
layout(location = 1) in vec2 aOffset;   // per-instance position

out vec4 vColor; // pass the color to the fragment shader

uniform vec2 uOrigin;
uniform vec4 uColor;
uniform mat4 uVP; // view projection matrix

void main() {
    vec2 pos = aPosition + aOffset + uOrigin;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);
    vColor = uColor;
}