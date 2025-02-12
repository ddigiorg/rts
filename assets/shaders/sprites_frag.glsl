#version 460 core

in vec2 fSpriteVertexTexCoord;
in vec2 fSpriteTextureOffset;
in vec2 fSpriteTextureSize;

out vec4 FragColor;

uniform sampler2D textureAtlas;

void main() {
    vec2 texCoord = fSpriteVertexTexCoord * fSpriteTextureSize + fSpriteTextureOffset;
    FragColor = texture(textureAtlas, texCoord);
}