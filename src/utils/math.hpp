#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp> // for distance2

#include <cmath>

// TODO: for flexibility, create functions with raw float parameters

// =============================================================================
// Orthoganal (Square) Tiles
// =============================================================================
// TODO: bitshift versions for faster conversion

inline glm::vec2 orthoTileToWorld(
        const glm::vec2& tile,
        const glm::vec2& size
) {
    float worldX = tile.x * size.x;
    float worldY = tile.y * size.y;
    return glm::vec2(worldX, tile.y * worldY);
}

inline glm::vec2 orthoWorldToTile(
        const glm::vec2& world,
        const glm::vec2& size
) {
    float tileX = std::floor(world.x / size.x);
    float tileY = std::floor(world.y / size.y);
    return glm::vec2(tileX, tileY);
}

// =============================================================================
// Isometric Tiles
// =============================================================================
// TODO: bitshift versions for faster conversion

inline glm::vec2 isoTileToWorld(
        const glm::vec2& tile,
        const glm::vec2& size
) {
    float worldX = (tile.x - tile.y) * (size.x * 0.5f);
    float worldY = (tile.y + tile.x) * (size.y * 0.5f);
    return glm::vec2(worldX, worldY);
}

inline glm::vec2 isoWorldToTile(
        const glm::vec2& world,
        const glm::vec2& size
) {
    float tileX = std::floor((world.x / size.x) + (world.y / size.y));
    float tileY = std::floor((world.y / size.y) - (world.x / size.x));
    return glm::vec2(tileX, tileY);
}

// =============================================================================
// Collision
// =============================================================================

inline bool checkAABBCollision(
    const glm::vec2& posA, const glm::vec2& sizeA,
    const glm::vec2& posB, const glm::vec2& sizeB
) {
    glm::vec2 maxA = posA + sizeA;
    glm::vec2 maxB = posB + sizeB;

    return !(maxA.x <= posB.x || posA.x >= maxB.x || 
             maxA.y <= posB.y || posA.y >= maxB.y);
}

inline bool checkCircleCollision(
    const glm::vec2& centerA,
    float radiusA,
    const glm::vec2& centerB,
    float radiusB
) {
    float distSquared = glm::distance2(centerA, centerB);
    float radiusSum = radiusA + radiusB;
    return distSquared < (radiusSum * radiusSum);
}