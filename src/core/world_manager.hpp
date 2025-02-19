#pragma once

#include "core/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/render/chunks_renderer.hpp"

// https://github.com/Auburn/FastNoiseLite
#include "Cpp/FastNoiseLite.h"

#include <vector>

using namespace Graphics;

namespace Core {

class WorldManager {
public:
    WorldManager();
    void render(const Camera& camera);

private:
    void _noise(TileData& tiles, float chunkX, float chunkY);

    FastNoiseLite fnl;
    ChunksRenderer chunkRenderer;

    // chunk data
    float chunkOffsetX;
    float chunkOffsetY;
    std::vector<Position> chunkPositions;
    std::vector<TileData> chunkTileDatas;
    // std::vector<SectorData> sectorData;
};

WorldManager::WorldManager() : chunkRenderer(CHUNK_RENDER_COUNT) {

    // setup fast noise lite
    fnl.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    // fnl.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    fnl.SetFrequency(0.015f);

    // calculate chunk offsets
    chunkOffsetX = std::floor(CHUNK_RENDER_COUNT_X * 0.5f);
    chunkOffsetY = std::floor(CHUNK_RENDER_COUNT_Y * 0.5f);

    // reserve space in vectors to avoid reallocations
    chunkPositions.reserve(CHUNK_RENDER_COUNT);
    chunkTileDatas.reserve(CHUNK_RENDER_COUNT);

    // initialize chunk data
    for (unsigned int i = 0; i < CHUNK_RENDER_COUNT; i++) {
        int x = i % CHUNK_RENDER_COUNT_X - chunkOffsetX;
        int y = i / CHUNK_RENDER_COUNT_X - chunkOffsetY;

        chunkPositions.emplace_back(chunkIndexToWorld(Index{x, y}));
        chunkTileDatas.emplace_back();
        chunkTileDatas.back().fill(1);
        // _noise(chunkTileDatas.back(), x, y);

        chunkRenderer.updateChunkPosition(i, &chunkPositions[i]);
        chunkRenderer.updateChunkTiles(i, &chunkTileDatas[i]);
    }
}

void WorldManager::_noise(TileData& tiles, float chunkX, float chunkY) {

    std::cout << chunkX << " " << chunkY << std::endl;

    for (unsigned int i = 0; i < CHUNK_TILE_COUNT; i++) {
        int x = i % CHUNK_TILE_COUNT_X;
        int y = i / CHUNK_TILE_COUNT_X;

        float tileX = chunkX * CHUNK_TILE_COUNT_X + x;
        float tileY = chunkY * CHUNK_TILE_COUNT_Y + y;

        if (i < 5)
            std::cout << tileX << " " << tileY << std::endl;

        float noise = fnl.GetNoise(tileX, tileY); // returns float between {-1, 1}
        unsigned int tile = (unsigned int)((noise + 1.0f)) + 1;

        tiles[i] = tile;
        // std::cout << noise << std::endl;
        // std::cout << tile << std::endl;
        // std::cout << std::endl;

    }
    std::cout << std::endl;
}

void WorldManager::render(const Camera& camera) {
    chunkRenderer.render(camera);
}

} // namespace Core