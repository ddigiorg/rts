#version 460 core

in vec2 fVertexTexCoord;
in flat uint fTileType; // must always use flat for integer types

out vec4 FragColor;

uniform sampler2D spritesheet;
uniform vec2 uVertexSize;
uniform vec2 uSheetSize;
uniform uint uTilesPerRow;

void main() {

    // TODO: can make this faster with bitshifts and sheets that are powers of 2
    // get tile sheet coordinates
    uint sheetX = fTileType % uTilesPerRow;
    uint sheetY = fTileType / uTilesPerRow;

    // compute tile texture coordinates
    vec2 tileUVSize = uVertexSize / uSheetSize;
    vec2 tileOffset = vec2(sheetX, sheetY) * tileUVSize;
    vec2 tileTexCoord = fVertexTexCoord * tileUVSize + tileOffset;

    FragColor = texture(spritesheet, tileTexCoord);
}