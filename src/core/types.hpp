#pragma once

#include <cstdint>
#include <string>

namespace Core {

// =============================================================================
// Aliases
// =============================================================================

using TileMask = uint8_t;

// =============================================================================
// Constants
// =============================================================================

constexpr const int TILE_PIXELS_X = 32;
constexpr const int TILE_PIXELS_Y = 32;

// =============================================================================
// Enums
// =============================================================================

enum class TileType : uint8_t {
    None,
    Grass,
    WaterShallow,
    Water,
};

// =============================================================================
// Structs
// =============================================================================

struct Position2f {
    float x, y;
};

struct Position2i {
    int x, y;
};

struct Velocity2f {
    float x, y;
};

struct Color4f {
    float r, g, b, a;
};

struct TextureRegion {
    float u, v, w, h;
};

struct BoundingBox {
    Position2f leftTop;
    Position2f rightBottom;
};

struct SpriteSheet {
    float sizeX; // pixels
    float sizeY; // pixels
    std::string filepath;
};

struct Tile {
    TileType type;
    TileMask mask;
    TextureRegion sprite; // TODO
};



} // namespace Core