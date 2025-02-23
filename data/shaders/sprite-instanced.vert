#version 460 core

layout(location = 0) in vec2 vPosition; // vertex (x, y)
layout(location = 1) in vec2 vTexCoord; // vertex (u, v)
layout(location = 2) in vec3 iPosition; // instance (x, y, z)
layout(location = 3) in vec2 iScale;    // instance (xw, yh)
layout(location = 4) in vec2 iTexCoord; // instance (u, v)
layout(location = 5) in vec2 iTexScale; // instance (uw, vh)
out vec2 fTexCoord;
uniform mat4 uVP; // view-projection matrix

void main() {

    // set position in world space
    vec2 posXY = vPosition * iScale + iPosition.xy;
    gl_Position = uVP * vec4(posXY, iPosition.z, 1.0);

    // pass variables to fragment shader
    fTexCoord = vTexCoord * iTexScale + iTexCoord;
}