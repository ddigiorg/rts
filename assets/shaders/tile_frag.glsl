#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    // FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    FragColor = texture(texture1, TexCoord);
}