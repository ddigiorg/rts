#version 460 core
in vec2 texcoord;

out vec4 color;

uniform sampler2D uTexture;

uniform vec3  uCharColor; // controls color
uniform float uCharWidth; // controls size of opaque area
uniform float uCharEdge;  // controls anti-ailiasing

uniform vec3  uBorderColor;  // controls color
uniform vec2  uBorderOffset; // controls drop shadow effect
uniform float uBorderWidth;  // controls size of opaque area
uniform float uBorderEdge;   // controls anti-ailiasing

void main()
{
    // using SDFs
    float charDistance = texture(uTexture, texcoord).r;
    float charAlpha = smoothstep(uCharWidth, uCharWidth + uCharEdge, charDistance);

    // TODO: why does border need 1.0 minus but not character???
    float borderDistance = 1.0 - texture(uTexture, texcoord + uBorderOffset).r;
    float borderAlpha = 1.0 - smoothstep(uBorderWidth, uBorderWidth + uBorderEdge, borderDistance);

    float totalAlpha = charAlpha + (1.0 - charAlpha) * borderAlpha;
    vec3 totalColor = mix(uBorderColor, uCharColor, charAlpha / totalAlpha);

    color = vec4(totalColor, totalAlpha);

    // // regular font rendering
    // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTexture, texcoord).r);
    // color = vec4(uCharColor, 1.0) * sampled;
}