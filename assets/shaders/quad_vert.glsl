#version 460 core
layout(location = 0) in vec2  aPos;    // quad vertex positions
layout(location = 1) in vec2  aOffset; // per-instance offsets (positions)
layout(location = 2) in float aSize;   // per-instance sizes
layout(location = 3) in vec3  aColor;  // Per-instance colors

out vec3 vColor; // pass the color to the fragment shader

uniform mat4 uVP; // view projection matrix

void main() {
    vColor = aColor;
    vec4 worldPosition = vec4((aPos * aSize/2) + aOffset, 0.0, 1.0);
    gl_Position = uVP * worldPosition;
}