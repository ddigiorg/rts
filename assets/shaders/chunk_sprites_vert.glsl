#version 460 core
layout(location = 0) in vec2 aPosition; // tile vertex position
layout(location = 1) in vec2 aTexCoord; // tile vertex texcoord
layout(location = 2) in vec2 aOffset;   // per-instance position

uniform vec2 uOrigin;
uniform mat4 uVP; // view projection matrix

out vec2 TexCoord;

void main() {
    vec2 pos = aPosition + aOffset + uOrigin;
    gl_Position = uVP * vec4(pos, 0.0, 1.0);
    TexCoord = aTexCoord;
}