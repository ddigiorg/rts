#pragma once

#include "core/components/position2i.hpp"
#include "core/components/position3f.hpp"
#include "core/components/scale2f.hpp"
#include "core/components/texcoord4f.hpp"

#include <array>
#include <vector>
#include <cstdint>
#include <cmath> // for std::floor
#include <string>

namespace Core {

// =============================================================================
// Constants
// =============================================================================

// // tile constants
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

// sprite sheet
constexpr const size_t SPRITE_SHEET_PIXELS_X = 384;
constexpr const size_t SPRITE_SHEET_PIXELS_Y = 192;
constexpr const float halfTexelX = 1.0f / float(SPRITE_SHEET_PIXELS_X);
constexpr const float halfTexelY = 1.0f / float(SPRITE_SHEET_PIXELS_Y);

// =============================================================================
// Components
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







// TODO: remove this, no longer needed
struct Transform {
    float x, y, w, h;
};

// =============================================================================
// Orthoganal Coordinate Functions
// =============================================================================
// TODO: bitshift versions for faster conversions

inline Position3f chunkGridToWorld(const Position2i& grid){
    float worldX = grid.x * CHUNK_SIZE_X;
    float worldY = grid.y * CHUNK_SIZE_Y;
    return Position3f(worldX, worldY, 0.0f);
}

inline Position2i chunkWorldToGrid(const Position3f& world){
    int indexX = std::floor(world.x / CHUNK_SIZE_X);
    int indexY = std::floor(world.y / CHUNK_SIZE_Y);
    return Position2i(indexX, indexY);
}

inline Position3f sectorGridToWorld(const Position2i& index){
    float worldX = index.x * SECTOR_SIZE_X;
    float worldY = index.y * SECTOR_SIZE_Y;
    return Position3f(worldX, worldY, 0.0f);
}

inline Position2i sectorWorldToGrid(const Position3f& world){
    int indexX = std::floor(world.x / SECTOR_SIZE_X);
    int indexY = std::floor(world.y / SECTOR_SIZE_Y);
    return Position2i(indexX, indexY);
}

inline Position3f tileGridToWorld(const Position2i& index){
    float worldX = index.x * TILE_SIZE_X;
    float worldY = index.y * TILE_SIZE_Y;
    return Position3f(worldX, worldY, 0.0f);
}

inline Position2i tileWorldToGrid(const Position3f& world){
    int indexX = std::floor(world.x / TILE_SIZE_X);
    int indexY = std::floor(world.y / TILE_SIZE_Y);
    return Position2i(indexX, indexY);
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
// TexCoord Functions
// =============================================================================

inline TexCoord4f computeTexCoord(int pu, int pv, int pw, int ph) {
    // NOTE: using half texel offsets to avoid texture bleeding
    float tu = (float(pu) + 0.5f) * halfTexelX;
    float tv = (float(pv) + 0.5f) * halfTexelY;
    float tw = (float(pw) - 1.0f) * halfTexelX;
    float th = (float(ph) - 1.0f) * halfTexelY;
    return TexCoord4f(tu, tv, tw, th);
}

// =============================================================================
// Collision Functions
// =============================================================================

inline bool detectAABBCollision(
        const Position3f& posA, const Scale2f& scaleA,
        const Position3f& posB, const Scale2f& scaleB
) {
    return !(posA.x + scaleA.x <= posB.x || posA.x >= posB.x + scaleB.x || 
             posA.y + scaleA.y <= posB.y || posA.y >= posB.y + scaleB.y);
}

inline void resolveCircleCollision(
        Position3f& posA, const float radiusA,
        Position3f& posB, const float radiusB
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

using Animation = std::vector<TexCoord4f>;
using AnimationOriented = std::array<Animation, 8>;

struct TileConfig {
    Animation frames;
};

static const TileConfig GRASS_CONFIG {
    /*frames:*/Animation{computeTexCoord( 64, 128,  64, 64)},
};

static const TileConfig WATER_CONFIG {
    /*frames:*/Animation{computeTexCoord(128, 128,  64, 64)},
};


static const std::array<TileConfig, 2> TILE_CONFIGS {
    GRASS_CONFIG,
    WATER_CONFIG,
};


struct UnitConfig {
    std::string name;
    Scale2f scale;
    Scale2f hitbox;
    AnimationOriented frames;
};

static const UnitConfig BALL_CYAN_CONFIG {
    /*name:*/ "Ball Cyan",
    /*scale:*/ Scale2f{32.0f, 32.0f},
    /*hitbox:*/ Scale2f{32.0f, 32.0f},
    /*frames:*/ AnimationOriented{
        /*facing 0:*/ Animation{
            /*frame 0:*/ computeTexCoord(128, 64,  64, 64),
        },
        /*facing 1:*/ Animation{
            /*frame 0:*/ computeTexCoord(192, 64,  64, 64),
        },
        /*facing 2:*/ Animation{
            /*frame 0:*/ computeTexCoord(256, 64,  64, 64),
        },
        /*facing 3:*/ Animation{
            /*frame 0:*/ computeTexCoord(256, 64, -64, 64),
        },
        /*facing 4:*/ Animation{
            /*frame 0:*/ computeTexCoord(192, 64, -64, 64),
        },
        /*facing 5:*/ Animation{
            /*frame 0:*/ computeTexCoord(128, 64, -64, 64),
        },
        /*facing 6:*/ Animation{
            /*frame 0:*/ computeTexCoord(  0, 64,  64, 64),
        },
        /*facing 7:*/ Animation{
            /*frame 0:*/ computeTexCoord( 64, 64,  64, 64),
        },
    },
};

static const UnitConfig BALL_MAGENTA_CONFIG {
    /*name:*/ "Ball Magenta",
    /*scale:*/ Scale2f{32.0f, 32.0f},
    /*hitbox:*/ Scale2f{32.0f, 32.0f},
    /*frames:*/ AnimationOriented{
        /*facing 0:*/ Animation {
            /*frame 0:*/ computeTexCoord(128, 0,  64, 64),
        },
        /*facing 1:*/ Animation{
            /*frame 0:*/ computeTexCoord(192, 0,  64, 64),
        },
        /*facing 2:*/ Animation{
            /*frame 0:*/ computeTexCoord(256, 0,  64, 64),
        },
        /*facing 3:*/ Animation{
            /*frame 0:*/ computeTexCoord(256, 0, -64, 64),
        },
        /*facing 4:*/ Animation{
            /*frame 0:*/ computeTexCoord(192, 0, -64, 64),
        },
        /*facing 5:*/ Animation{
            /*frame 0:*/ computeTexCoord(128, 0, -64, 64),
        },
        /*facing 6:*/ Animation{
            /*frame 0:*/ computeTexCoord(  0, 0,  64, 64),
        },
        /*facing 7:*/ Animation{
            /*frame 0:*/ computeTexCoord( 64, 0,  64, 64),
        },
    },
};

enum UnitType {
    BALL_CYAN,
    BALL_MAGENTA,
};

// TODO: should this be in UnitManager?
static const std::array<UnitConfig, 2> UNIT_CONFIGS = {
    BALL_CYAN_CONFIG,
    BALL_MAGENTA_CONFIG,
};

} // namespace Core