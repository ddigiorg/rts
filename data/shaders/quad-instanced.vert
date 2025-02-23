#version 460 core

layout(location = 0) in vec2 vPosition; // vertex (x, y)
layout(location = 1) in vec3 iPosition; // instance (x, y, z)
layout(location = 2) in vec2 iScale;    // instance (xw, yh)
layout(location = 3) in vec4 iColor;    // instance (r, g, b, a)
out vec4 vColor;
uniform mat4 uVP; // view projection matrix

void main() {

    // calculate vertex position
    vec2 posXY = vPosition * iScale + iPosition.xy;
    gl_Position = uVP * vec4(posXY, iPosition.z, 1.0);

    // pass variables to fragment shader
    vColor = iColor;
}