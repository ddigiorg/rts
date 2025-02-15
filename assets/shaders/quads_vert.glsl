#version 460 core

layout(location = 0) in vec2 aQuadVertexPosition; // TODO: change to aVertexPosition
layout(location = 1) in vec2 aQuadOffset; // TODO: change to aWorldPosition
layout(location = 2) in vec2 aQuadSize; // TODO: change to aScale
layout(location = 3) in vec3 aQuadColor; // TODO: change to aColor

out vec3 fQuadColor;

uniform mat4 uVP; // view projection matrix

void main() {

    // calculate quad position
    vec2 pos = (aQuadVertexPosition * aQuadSize * 0.5) + aQuadOffset;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // pass variables to fragment shader
    fQuadColor = aQuadColor;
}