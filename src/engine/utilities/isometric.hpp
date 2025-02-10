#pragma once

#include <glm/glm.hpp>
#include <cmath>

// https://pikuma.com/blog/isometric-projection-in-games

inline glm::vec2 IsoToWorld(const glm::vec2& isoPos, const glm::vec2& isoSize) {
    float worldX = (isoPos.x - isoPos.y) * (isoSize.x * 0.5f);
    float worldY = (isoPos.y + isoPos.x) * (isoSize.y * 0.5f);
    return glm::vec2(worldX, worldY);
}

inline glm::vec2 WorldToIso(const glm::vec2& worldPos, const glm::vec2& isoSize) {
    float tileX = std::floor((worldPos.x / isoSize.x) + (worldPos.y / isoSize.y));
    float tileY = std::floor((worldPos.y / isoSize.y) - (worldPos.x / isoSize.x));
    return glm::vec2(tileX, tileY);
}


