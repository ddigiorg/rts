#version 460 core

layout(location = 0) in vec2 aSpriteVertexPosition;
layout(location = 1) in vec2 aSpriteVertexTexCoord;
layout(location = 2) in vec2 aSpritePosition;
layout(location = 3) in vec2 aSpriteScale;
layout(location = 4) in vec2 aSpriteTexCoord;
layout(location = 5) in vec2 aSpriteTexScale;

out vec2 fSpriteVertexTexCoord;
out vec2 fSpriteTexCoord;
out vec2 fSpriteTexScale;

uniform vec2 uYBounds; // y-axis boundaries for normalizing z-depth
uniform mat4 uVP; // view-projection matrix

void main() {

    // Calculate sprite position in world space
    vec2 pos = (aSpriteVertexPosition * aSpriteScale) + aSpritePosition;
    float z = (uYBounds.y - aSpritePosition.y) / (uYBounds.y - uYBounds.x); // normalize z-depth
    gl_Position = uVP * vec4(pos, z, 1.0);

    // Pass texture variables to fragment shader
    fSpriteVertexTexCoord = aSpriteVertexTexCoord;
    fSpriteTexCoord = aSpriteTexCoord;
    fSpriteTexScale = aSpriteTexScale;
}