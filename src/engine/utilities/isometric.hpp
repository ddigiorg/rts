#pragma once

#include <glm/glm.hpp>
#include <cmath>

// https://pikuma.com/blog/isometric-projection-in-games

inline glm::vec2 tileToWorld(const glm::ivec2& tilePos, const glm::ivec2& tileSize) {
    float worldX = (tilePos.x - tilePos.y) * (tileSize.x * 0.5f);
    float worldY = (tilePos.y + tilePos.x) * (tileSize.y * 0.5f);
    return glm::vec2(worldX, worldY);
}

inline glm::ivec2 worldToTile(const glm::vec2& worldPos, const glm::ivec2& tileSize) {
    int tileX = static_cast<int>(std::floor((worldPos.x / tileSize.x + worldPos.y / tileSize.y)));
    int tileY = static_cast<int>(std::floor((worldPos.y / tileSize.y - worldPos.x / tileSize.x)));
    return glm::ivec2(tileX, tileY);
}


