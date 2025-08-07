#version 460 core

layout(location = 0) in vec2 vPosition; // vertex position (x, y)
layout(location = 1) in vec4 iBound;    // instance bound (minX, minY, maxX, maxY)
layout(location = 2) in vec4 iColor;    // instance color (r, g, b, a)
out vec4 vColor;
uniform mat4 uVP; // view projection matrix

void main() {

    // calculate vertex position
    vec2 size = iBound.zw - iBound.xy;
    vec2 worldPos = iBound.xy + vPosition * size;
    gl_Position = uVP * vec4(worldPos, 0.0, 1.0);

    // pass variables to fragment shader
    vColor = iColor;
}