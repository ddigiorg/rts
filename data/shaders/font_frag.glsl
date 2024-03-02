#version 460 core

in vec2 texcoord;
out vec4 color;

uniform sampler2D u_texture;

uniform vec3  u_charColor;    // controls color
uniform float u_charWidth;    // controls size of opaque area
uniform float u_charEdge;     // controls anti-ailiasing

uniform vec3  u_borderColor;  // controls color
uniform vec2  u_borderOffset; // controls drop shadow effect
uniform float u_borderWidth;  // controls size of opaque area
uniform float u_borderEdge;   // controls anti-ailiasing

void main()
{
    // using SDFs
    float charDistance = texture(u_texture, texcoord).r;
    float charAlpha = smoothstep(u_charWidth, u_charWidth + u_charEdge, charDistance);

    // TODO: why does border need 1.0 minus but not character???
    float borderDistance = 1.0 - texture(u_texture, texcoord + u_borderOffset).r;
    float borderAlpha = 1.0 - smoothstep(u_borderWidth, u_borderWidth + u_borderEdge, borderDistance);

    float totalAlpha = charAlpha + (1.0 - charAlpha) * borderAlpha;
    vec3 totalColor = mix(u_borderColor, u_charColor, charAlpha / totalAlpha);

    color = vec4(totalColor, totalAlpha);

    // // regular font rendering
    // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, texcoord).r);
    // color = vec4(u_charColor, 1.0) * sampled;
}