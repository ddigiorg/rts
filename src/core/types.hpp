#pragma once

#include <yaml-cpp/yaml.h>

#include <array>
#include <vector>
#include <cstdint>
#include <cmath> // for std::floor
#include <string>

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

// TODO: remove this, no longer needed
struct Transform {
    float x, y, w, h;
};

struct Color {
    float r, g, b, a;
};

struct TexCoord {
    float u, v, w, h;
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

// inline void resolveCircleCollision(CircleCollider& a, CircleCollider& b) {
inline void resolveCircleCollision(
        Position& posA, const float radiusA,
        Position& posB, const float radiusB
) {
    float dx = posB.x - posA.x;
    float dy = posB.y - posA.y;
    float distSquared = (dx * dx) + (dy * dy);
    float radiusSum = radiusA + radiusB;

    // if no collision then exit
    if (distSquared >= (radiusSum * radiusSum)) return;

    // compute distance
    float dist = std::sqrt(distSquared);

    // TODO: As an added speed hack, you can calculate the distance with
    // Pythagoras' theorem, but don't calculate the expensive square root,
    // just keep your total centre-to-centre distance and compare to the 
    // square of the sum of the radii. 

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
    posA.x -= nx * correction;
    posA.y -= ny * correction;
    posB.x += nx * correction;
    posB.y += ny * correction;
}

// =============================================================================
// Game Object Configs
// =============================================================================
constexpr const size_t SPRITE_SHEET_PIXELS_X = 384;
constexpr const size_t SPRITE_SHEET_PIXELS_Y = 192;
constexpr const float halfTexelX = 1.0f / float(SPRITE_SHEET_PIXELS_X);
constexpr const float halfTexelY = 1.0f / float(SPRITE_SHEET_PIXELS_Y);


// StaticSprite: no frames, no facings (tiles, static objects)
// LoopingSprite: frames, no facings (buildings, sparkling resources)
// OrientedSprite: frames and facings (units)
using SpriteTableOriented = std::vector<std::array<TexCoord, 8>>; // Note: indexed by [FRAME][FACING]
using SpriteTable = std::vector<TexCoord>;

struct UnitConfig {
    std::string name;
    Scale hitboxScale;
    Scale spriteScale;
    SpriteTableOriented idleSprites;
};

constexpr TexCoord computeTexCoord(int pu, int pv, int pw, int ph) {
    // NOTE: using half texel offsets to avoid texture bleeding
    float tu = (float(pu) + 0.5f) * halfTexelX;
    float tv = (float(pv) + 0.5f) * halfTexelY;
    float tw = (float(pw) - 1.0f) * halfTexelX;
    float th = (float(ph) - 1.0f) * halfTexelY;
    return TexCoord{tu, tv, tw, th};
}

UnitConfig loadUnitConfig(const std::string& filepath) {
    YAML::Node config = YAML::LoadFile(filepath);

    UnitConfig unit;
    unit.name = config["name"].as<std::string>();

    auto hitbox = config["hitboxScale"];
    unit.hitboxScale = {hitbox[0].as<float>(), hitbox[1].as<float>()};

    auto sprite = config["spriteScale"];
    unit.spriteScale = {sprite[0].as<float>(), sprite[1].as<float>()};

    auto idleSprites = config["idleSprites"];
    size_t frameCount = idleSprites.size();
    unit.idleSprites.resize(frameCount);

    for (size_t frame = 0; frame < frameCount; frame++) {
        for (size_t facing = 0; facing < 8; facing++) {
            const auto& idleSprite = idleSprites[frame][facing];
            unit.idleSprites[frame][facing] = computeTexCoord(
                idleSprite[0].as<int>(), // u
                idleSprite[1].as<int>(), // v
                idleSprite[2].as<int>(), // w
                idleSprite[3].as<int>()  // h
            );
        }
    }

    return unit;
}

enum UnitType {
    BALL_CYAN,
    BALL_MAGENTA,
};

// TODO: should this be in UnitManager?
static const std::array<UnitConfig, 2> UNIT_CONFIGS = {
    loadUnitConfig("data/configs/unit-ball-cyan.yaml"), // BALL_CYAN
    loadUnitConfig("data/configs/unit-ball-magenta.yaml"), // BALL_MAGENTA
};

} // namespace Core