#version 460 core

in vec3 fQuadColor;

out vec4 FragColor;

void main() {
    FragColor = vec4(fQuadColor, 1.0);
}