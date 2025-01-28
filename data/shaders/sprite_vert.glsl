#version 460 core
layout(location = 0) in vec2 aPos;    // quad vertex positions
layout(location = 1) in vec2 aOffset; // per-instance offsets (sprite positions)

uniform mat4 uVP; // view projection matrix

void main() {
    vec4 worldPosition = vec4(aPos + aOffset, 0.0, 1.0);
    gl_Position = uVP * worldPosition;
}


// layout (location=0) in vec2 vPosVS;

// uniform mat4 mvp;

// void main() {
//     gl_Position = mvp * vec4(vPosVS, 0.0, 1.0);
// }