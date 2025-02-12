#version 460 core

layout(location = 0) in vec2 aSpriteVertexPosition;
layout(location = 1) in vec2 aSpriteVertexTexCoord;
layout(location = 2) in vec2 aSpriteWorldOffset;
layout(location = 3) in vec2 aSpriteWorldSize;
layout(location = 4) in vec2 aSpriteTextureOffset;
layout(location = 5) in vec2 aSpriteTextureSize;

out vec2 fSpriteVertexTexCoord;
out vec2 fSpriteTextureOffset;
out vec2 fSpriteTextureSize;

uniform mat4 uVP; // view-projection matrix

void main() {

    // Calculate sprite position in world space
    vec2 pos = (aSpriteVertexPosition * aSpriteWorldSize) + aSpriteWorldOffset;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);

    // Pass texture variables to fragment shader
    fSpriteVertexTexCoord = aSpriteVertexTexCoord;
    fSpriteTextureOffset = aSpriteTextureOffset;
    fSpriteTextureSize = aSpriteTextureSize;
}