#version 460 core

in vec2 fSpriteVertexTexCoord;
in vec2 fSpriteTexCoord;
in vec2 fSpriteTexScale;

out vec4 FragColor;

uniform sampler2D textureAtlas;

void main() {
    vec2 texCoord = fSpriteVertexTexCoord * fSpriteTexScale + fSpriteTexCoord;
    vec4 color = texture(textureAtlas, texCoord);

    // prevent depth writes for transparent pixels
    if (color.a < 0.1) discard;

    FragColor = color;
}