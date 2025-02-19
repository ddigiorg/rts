#version 460 core

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexTexCoord;
layout(location = 2) in vec2 aPosition;
layout(location = 3) in uint aTileType;
layout(location = 4) in vec2 aOffset;

out vec2 fVertexTexCoord;
out uint fTileType;

uniform vec2 uVertexSize;
uniform mat4 uVP; // view projection matrix

void main() {

    // calculate tile vertex position
    vec2 pos = aVertexPosition * uVertexSize + aPosition + aOffset;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // pass variables to fragment shader
    fVertexTexCoord = aVertexTexCoord;
    fTileType = aTileType;
}