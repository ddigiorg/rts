#version 460 core

in vec2 fTexCoord;
out vec4 FragColor;
uniform sampler2D atlasTexture;

void main() {

    // set color
    vec4 color = texture(atlasTexture, fTexCoord);

    // prevent depth writes for transparent pixels
    if (color.a < 0.05) discard;

    FragColor = color;
}