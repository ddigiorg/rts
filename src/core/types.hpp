#pragma once

#include <array>
#include <cstdint>
#include <cmath> // for std::floor

namespace Core {

// =============================================================================
// Constants
// =============================================================================

// tile constants
constexpr uint16_t TILE_SIZE_X = 32; // pixels
constexpr uint16_t TILE_SIZE_Y = 32; // pixels

// sector constants
constexpr uint16_t SECTOR_TILE_COUNT_X = 4; // tiles
constexpr uint16_t SECTOR_TILE_COUNT_Y = 4; // tiles
constexpr uint16_t SECTOR_TILE_COUNT = SECTOR_TILE_COUNT_X * SECTOR_TILE_COUNT_Y;
constexpr uint16_t SECTOR_SIZE_X = TILE_SIZE_X * SECTOR_TILE_COUNT_X; // pixels
constexpr uint16_t SECTOR_SIZE_Y = TILE_SIZE_Y * SECTOR_TILE_COUNT_Y; // pixels

// chunk constants
constexpr uint16_t CHUNK_TILE_COUNT_X = 32; // tiles
constexpr uint16_t CHUNK_TILE_COUNT_Y = 32; // tiles
constexpr uint16_t CHUNK_TILE_COUNT = CHUNK_TILE_COUNT_X * CHUNK_TILE_COUNT_Y;
constexpr uint16_t CHUNK_SECTOR_COUNT_X = CHUNK_TILE_COUNT_X / SECTOR_TILE_COUNT_X; // sectors
constexpr uint16_t CHUNK_SECTOR_COUNT_Y = CHUNK_TILE_COUNT_Y / SECTOR_TILE_COUNT_Y; // sectors
constexpr uint16_t CHUNK_SECTOR_COUNT = CHUNK_SECTOR_COUNT_X * CHUNK_SECTOR_COUNT_Y;
constexpr uint16_t CHUNK_SIZE_X = TILE_SIZE_X * CHUNK_TILE_COUNT_X; // pixels
constexpr uint16_t CHUNK_SIZE_Y = TILE_SIZE_Y * CHUNK_TILE_COUNT_Y; // pixels

constexpr uint16_t CHUNK_RENDER_RADIUS = 2;

// =============================================================================
// Components
// =============================================================================

// TODO: consider changing to Position2i
struct Location {
    int x, y;
};

// TODO: consider changing to Position3f
struct Position {
    float x, y, z;
};

struct Scale {
    float x, y;
};

struct Velocity {
    float x, y;
};

struct Transform {
    float x, y, w, h;
};

struct Color {
    float r, g, b, a;
};

struct TexCoord {
    float u, v, w, h;
};

struct CircleCollider {
    float x, y; // center of circle
    float radius;
};

// =============================================================================
// Types
// =============================================================================

using TileData = std::array<uint8_t, CHUNK_TILE_COUNT>;

// =============================================================================
// Orthoganal Coordinate Functions
// =============================================================================
// TODO: bitshift versions for faster conversions

inline Position chunkGridToWorld(const Location& index){
    float worldX = index.x * CHUNK_SIZE_X;
    float worldY = index.y * CHUNK_SIZE_Y;
    return Position{worldX, worldY, 0.0f};
}

inline Location chunkWorldToGrid(const Position& world){
    int indexX = std::floor(world.x / CHUNK_SIZE_X);
    int indexY = std::floor(world.y / CHUNK_SIZE_Y);
    return Location{indexX, indexY};
}

inline Position sectorGridToWorld(const Location& index){
    float worldX = index.x * SECTOR_SIZE_X;
    float worldY = index.y * SECTOR_SIZE_Y;
    return Position{worldX, worldY, 0.0f};
}

inline Location sectorWorldToGrid(const Position& world){
    int indexX = std::floor(world.x / SECTOR_SIZE_X);
    int indexY = std::floor(world.y / SECTOR_SIZE_Y);
    return Location{indexX, indexY};
}

inline Position tileGridToWorld(const Location& index){
    float worldX = index.x * TILE_SIZE_X;
    float worldY = index.y * TILE_SIZE_Y;
    return Position{worldX, worldY, 0.0f};
}

inline Location tileWorldToGrid(const Position& world){
    int indexX = std::floor(world.x / TILE_SIZE_X);
    int indexY = std::floor(world.y / TILE_SIZE_Y);
    return Location{indexX, indexY};
}

// =============================================================================
// Isometric Coordinate Functions
// =============================================================================
// TODO: bitshift versions for faster conversions

// inline Position tileGridToWorld(const Location& index){
//     float worldX = (index.x - index.y) * (TILE_SIZE_X * 0.5f);
//     float worldY = (index.y + index.x) * (TILE_SIZE_Y * 0.5f);
//     return Position{worldX, worldY};
// }

// inline Location tileWorldToGrid(const Position& world){
//     int indexX = std::floor((world.x / TILE_SIZE_X) + (world.y / TILE_SIZE_Y));
//     int indexY = std::floor((world.y / TILE_SIZE_Y) - (world.x / TILE_SIZE_X));
//     return Location{indexX, indexY};
// }

// =============================================================================
// Collision Functions
// =============================================================================

inline bool detectAABBCollision(
    const Position& posA, const Scale& scaleA,
    const Position& posB, const Scale& scaleB
) {
    return !(posA.x + scaleA.x <= posB.x || posA.x >= posB.x + scaleB.x || 
             posA.y + scaleA.y <= posB.y || posA.y >= posB.y + scaleB.y);
}

inline bool detectCircleCollision(const CircleCollider& a, const CircleCollider& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float distSquared = (dx * dx) + (dy * dy);
    float radiusSum = a.radius + b.radius;
    return distSquared < (radiusSum * radiusSum);
}

inline void resolveCircleCollision(CircleCollider& a, CircleCollider& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float distSquared = (dx * dx) + (dy * dy);
    float radiusSum = a.radius + b.radius;

    // if no collision then exit
    if (distSquared >= (radiusSum * radiusSum)) return;

    // compute distance
    float dist = std::sqrt(distSquared);
    
    // avoid division by zero for perfectly overlapping circles
    constexpr float epsilon = 1e-6f;
    if (dist < epsilon) {
        dx = 1.0f; // arbitrary push direction
        dy = 0.0f;
        dist = 1.0f;
    }

    // compute penetration depth (overlap distance)
    float overlap = radiusSum - dist;

    // normalize the displacement vector
    float invDist = 1.0f / dist;
    float nx = dx * invDist;
    float ny = dy * invDist;

    // push both units away proportionally
    float correction = overlap * 0.5f;
    a.x -= nx * correction;
    a.y -= ny * correction;
    b.x += nx * correction;
    b.y += ny * correction;
}

// =============================================================================
// Sprite Texture Coordinates
// =============================================================================

constexpr const size_t SPRITE_TYPE_COUNT = 4;
constexpr const size_t SPRITE_COUNT = 20;
constexpr const size_t SPRITE_SHEET_PIXELS_X = 384;
constexpr const size_t SPRITE_SHEET_PIXELS_Y = 192;

constexpr TexCoord computeTexCoord(int x, int y, int w, int h) {
    constexpr float texelX = 1.0f / float(SPRITE_SHEET_PIXELS_X);
    constexpr float texelY = 1.0f / float(SPRITE_SHEET_PIXELS_Y);

    // using half texel offsets to avoid texture bleeding
    float u = (float(x) + 0.5f) * texelX;
    float v = (float(y) + 0.5f) * texelY;
    float uw = (float(w) - 1.0f) * texelX;
    float vh = (float(h) - 1.0f) * texelY;

    return TexCoord{u, v, uw, vh};
}

enum SpriteType {
    TERRAIN,
    UNIT_CYAN,
    UNIT_MAGENTA,
    UNIT_TREE,
};

constexpr const std::array<size_t, SPRITE_TYPE_COUNT> SPRITE_OFFSETS = {
    0,  // TERRAIN
    3,  // UNIT_CYAN
    11, // UNIT_MAGENTA
    12, // UNIT_TREE
};

enum Sprite {
    // terrain
    SPRITE_TERRAIN_NULL,
    SPRITE_TERRAIN_GRASS,
    SPRITE_TERRAIN_WATER,
    // cyan unit
    SPRITE_UNIT_CYAN_000,
    SPRITE_UNIT_CYAN_045,
    SPRITE_UNIT_CYAN_090,
    SPRITE_UNIT_CYAN_135,
    SPRITE_UNIT_CYAN_180,
    SPRITE_UNIT_CYAN_225,
    SPRITE_UNIT_CYAN_270,
    SPRITE_UNIT_CYAN_315,
    // magenta unit
    SPRITE_UNIT_MAGENTA_000,
    SPRITE_UNIT_MAGENTA_045,
    SPRITE_UNIT_MAGENTA_090,
    SPRITE_UNIT_MAGENTA_135,
    SPRITE_UNIT_MAGENTA_180,
    SPRITE_UNIT_MAGENTA_225,
    SPRITE_UNIT_MAGENTA_270,
    SPRITE_UNIT_MAGENTA_315,
    // tree
    SPRITE_UNIT_TREE,
};

constexpr const std::array<TexCoord, SPRITE_COUNT> SPRITE_TEXCOORDS = {{
    // terrain
    computeTexCoord(  0, 128,  64, 64), // SPRITE_TERRAIN_NULL
    computeTexCoord( 64, 128,  64, 64), // SPRITE_TERRAIN_GRASS
    computeTexCoord(128, 128,  64, 64), // SPRITE_TERRAIN_WATER
    // cyan
    computeTexCoord(128, 64,  64, 64), // SPRITE_UNIT_CYAN_000
    computeTexCoord(192, 64,  64, 64), // SPRITE_UNIT_CYAN_045
    computeTexCoord(256, 64,  64, 64), // SPRITE_UNIT_CYAN_090
    computeTexCoord(256, 64, -64, 64), // SPRITE_UNIT_CYAN_135
    computeTexCoord(192, 64, -64, 64), // SPRITE_UNIT_CYAN_180
    computeTexCoord(128, 64, -64, 64), // SPRITE_UNIT_CYAN_225
    computeTexCoord(  0, 64,  64, 64), // SPRITE_UNIT_CYAN_270
    computeTexCoord( 64, 64,  64, 64), // SPRITE_UNIT_CYAN_315
    // magenta
    computeTexCoord(128,  0,  64, 64), // SPRITE_UNIT_MAGENTA_000
    computeTexCoord(192,  0,  64, 64), // SPRITE_UNIT_MAGENTA_045
    computeTexCoord(256,  0,  64, 64), // SPRITE_UNIT_MAGENTA_090
    computeTexCoord(256,  0, -64, 64), // SPRITE_UNIT_MAGENTA_135
    computeTexCoord(192,  0, -64, 64), // SPRITE_UNIT_MAGENTA_180
    computeTexCoord(128,  0, -64, 64), // SPRITE_UNIT_MAGENTA_225
    computeTexCoord(  0,  0,  64, 64), // SPRITE_UNIT_MAGENTA_270
    computeTexCoord( 64,  0,  64, 64), // SPRITE_UNIT_MAGENTA_315
    // tree
    computeTexCoord(320,  0,  64, 128), // SPRITE_UNIT_TREE
}};

} // namespace Core