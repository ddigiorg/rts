#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 texcoord;

uniform mat4 uMatrix;

void main() {
    gl_Position = uMatrix * vec4(vertex.xy, 1.0, 1.0);
    texcoord = vertex.zw;
}