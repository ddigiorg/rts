#version 460 core

in vec4 vColor;     // vertex color (r, g, b, a)
out vec4 FragColor; // fragment shader output color

void main() {
    FragColor = vColor;
}