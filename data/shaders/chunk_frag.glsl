#version 460 core

in vec2 vTexFS;

out vec4 color;

uniform sampler2D spritesheet;

void main() {
    color = texture(spritesheet, vTexFS);
}