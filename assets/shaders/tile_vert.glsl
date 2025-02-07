#version 460 core
layout(location = 0) in vec2 aPosition; // tile vertex position
layout(location = 1) in vec2 aTexCoord; // tile vertex texcoord
layout(location = 2) in vec2 aOffset;   // per-instance position

uniform mat4 uVP; // view projection matrix
uniform float uTileSize; // tile size

out vec2 TexCoord;

void main() {
    vec2 isoPos = vec2(
        (aOffset.x - aOffset.y) * 0.5,
        (aOffset.x + aOffset.y) * 0.25
    );

    gl_Position = uVP * vec4((aPosition + isoPos) * uTileSize, 0.0, 1.0);
    TexCoord = aTexCoord;
}