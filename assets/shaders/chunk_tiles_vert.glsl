#version 460 core

layout(location = 0) in vec2 aTileVertexPosition;
layout(location = 1) in vec2 aTileVertexTexCoord;
layout(location = 2) in vec2 aTilePosition;
layout(location = 3) in uint aTileType;
layout(location = 4) in vec2 aChunkPosition;

out vec2 fTileVertexTexCoord;
out uint fTileType;

uniform mat4 uVP; // view projection matrix

void main() {

    // calculate tile vertex position
    vec2 pos = aTileVertexPosition + aTilePosition + aChunkPosition;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // pass variables to fragment shader
    fTileVertexTexCoord = aTileVertexTexCoord;
    fTileType = aTileType;
}