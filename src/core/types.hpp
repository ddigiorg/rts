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

constexpr uint16_t CHUNK_RENDER_COUNT_X = 3;
constexpr uint16_t CHUNK_RENDER_COUNT_Y = 3;
constexpr uint16_t CHUNK_RENDER_COUNT = CHUNK_RENDER_COUNT_X * CHUNK_RENDER_COUNT_Y;

// =============================================================================
// Components
// =============================================================================

struct Index {
    int x, y;
};

struct Position {
    float x, y;
};

struct Size {
    float x, y;
};

struct Velocity {
    float x, y;
};

struct Transform {
    float x, y, w, h;
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

inline Position chunkIndexToWorld(const Index& index){
    float worldX = index.x * CHUNK_SIZE_X;
    float worldY = index.y * CHUNK_SIZE_Y;
    return Position{worldX, worldY};
}

inline Index chunkWorldToIndex(const Position& world){
    int indexX = std::floor(world.x / CHUNK_SIZE_X);
    int indexY = std::floor(world.y / CHUNK_SIZE_Y);
    return Index{indexX, indexY};
}

inline Position sectorIndexToWorld(const Index& index){
    float worldX = index.x * SECTOR_SIZE_X;
    float worldY = index.y * SECTOR_SIZE_Y;
    return Position{worldX, worldY};
}

inline Index sectorWorldToIndex(const Position& world){
    int indexX = std::floor(world.x / SECTOR_SIZE_X);
    int indexY = std::floor(world.y / SECTOR_SIZE_Y);
    return Index{indexX, indexY};
}

inline Position tileIndexToWorld(const Index& index){
    float worldX = index.x * TILE_SIZE_X;
    float worldY = index.y * TILE_SIZE_Y;
    return Position{worldX, worldY};
}

inline Index tileWorldToIndex(const Position& world){
    int indexX = std::floor(world.x / TILE_SIZE_X);
    int indexY = std::floor(world.y / TILE_SIZE_Y);
    return Index{indexX, indexY};
}

// =============================================================================
// Isometric Coordinate Functions
// =============================================================================
// TODO: bitshift versions for faster conversions

// inline Position tileIndexToWorld(const Index& index){
//     float worldX = (index.x - index.y) * (TILE_SIZE_X * 0.5f);
//     float worldY = (index.y + index.x) * (TILE_SIZE_Y * 0.5f);
//     return Position{worldX, worldY};
// }

// inline Index tileWorldToIndex(const Position& world){
//     int indexX = std::floor((world.x / TILE_SIZE_X) + (world.y / TILE_SIZE_Y));
//     int indexY = std::floor((world.y / TILE_SIZE_Y) - (world.x / TILE_SIZE_X));
//     return Index{indexX, indexY};
// }

// =============================================================================
// Collision Functions
// =============================================================================

inline bool detectAABBCollision(const Transform& a, const Transform& b) {
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w || 
             a.y + a.h <= b.y || a.y >= b.y + b.h);
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

} // namespace Core