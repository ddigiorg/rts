#pragma once

#include "core/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/render/sprite_renderer.hpp"
#include "graphics/render/quad_renderer.hpp"

// https://github.com/Auburn/FastNoiseLite
#include "Cpp/FastNoiseLite.h"

#include <vector>
#include <iostream>

using namespace Graphics;

namespace Core {

class WorldManager {
public:
    WorldManager();
    void update(); // include delta time
    void render(const Camera& camera);

private:
    // void _setCapacity(const size_t newCapacity);
    // void _initChunk(const size_t idx, const Location& loc);
    // void _loadChunk(const size_t idx, const Location& loc);
    // void _saveChunk(const size_t idx, const Location& loc);

    // const Scale cScale = Scale{CHUNK_SIZE_X, CHUNK_SIZE_Y};
    // const Scale tScale = Scale{TILE_SIZE_X, TILE_SIZE_Y};
    // const Color cGridColor = Color{0.0f, 0.0f, 1.0f, 1.0f};
    // const Color tGridColor = Color{0.0f, 0.0f, 0.0f, 0.4f};
    // const float cLineWidth = 3.0f;
    // const float tLineWidth = 1.0f;

    void _setupChunkData();
    void _setupTileData();
    void _setupRenderers();

    void _initChunk(const size_t c, const Location& loc);
    // void _loadChunk(const size_t c, const Location& loc);
    // void _saveChunk(const size_t c);

    void _updateRenderers();

    // TODO: If your rendering layer frequently reads from your vectors, consider 
    // using persistent mapped buffers or double-buffering for GPU updates. This
    // will prevent unnecessary CPU-GPU sync stalls.
    // renderers
    SpriteRenderer spriteRenderer;
    // QuadRenderer tGridRenderer;
    // QuadRenderer cGridRenderer;

    // player data

    // chunk data
    size_t cCount;
    std::vector<Location> cLocations;
    std::vector<Position> cPositions;
    std::vector<size_t> cTileIdxs;

    // renderable chunk data
    // size_t rcCount;
    // std::vector<size_t> rcIdxs;

    // tile data
    size_t tCount;
    std::vector<TileType> tTypes;
    std::vector<TileMask> tMasks;

    // renderable tile data
    size_t rtCount;
    std::vector<Position> rtPositions;
    std::vector<TexCoord> rtTexCoords;
    std::vector<Scale> rtScales;

    // static object data
    size_t sCount;
    // std::vector<ObjectID> sIDs;
    // std::vector<StaticTypes> sTypes;
    std::vector<Position> sPositions;
    std::vector<TexCoord> sTexCoords;
    std::vector<Scale> sScales;

    // dynamic object data
    size_t dCount;

    // uniform grid cell data
    // store indicies of static and dynamic objects

};

WorldManager::WorldManager() {
    _setupChunkData();
    _setupTileData();
    _setupRenderers();

    _initChunk(0, {-1, -1});
    _initChunk(1, { 0, -1});
    _initChunk(2, { 1, -1});
    _initChunk(3, {-1,  0});
    _initChunk(4, { 0,  0});
    _initChunk(5, { 1,  0});
    _initChunk(6, {-1,  1});
    _initChunk(7, { 0,  1});
    _initChunk(8, { 1,  1});

    _updateRenderers();
}

void WorldManager::update() {
    // _updateRenderers();
}

void WorldManager::render(const Camera& camera) {
    spriteRenderer.render(camera);
    // tGridRenderer.renderOutline(camera, tLineWidth);
    // cGridRenderer.renderOutline(camera, cLineWidth);
}

void WorldManager::_setupChunkData() {
    cCount = 9;

    cLocations.resize(cCount);
    cPositions.resize(cCount);
    cTileIdxs.resize(cCount);
}

void WorldManager::_setupTileData() {
    tCount = cCount * CHUNK_TILE_COUNT;

    tTypes.resize(tCount);
    tMasks.resize(tCount);

    rtCount = tCount;

    rtPositions.resize(rtCount);
    rtTexCoords.resize(rtCount);
    rtScales.resize(rtCount);
}

void WorldManager::_setupRenderers() {
    size_t spriteCount = tCount;
    spriteRenderer.setCapacity(tCount);
}

void WorldManager::_initChunk(const size_t c, const Location& loc) {
    const Position pos = chunkGridToWorld(loc);
    const size_t t0 = c * CHUNK_TILE_COUNT;

    // set chunk data
    cLocations[c] = loc;
    cPositions[c] = pos;
    cTileIdxs[c] = t0;

    // set chunk tile data
    size_t i = 0;
    for (size_t y = 0; y < CHUNK_TILE_COUNT_Y; y++) {
        for (size_t x = 0; x < CHUNK_TILE_COUNT_X; x++) {
            size_t t = t0 + i++;
            float tPosX = (float(x) * TILE_SIZE_X) + pos.x;
            float tPosY = (float(y) * TILE_SIZE_Y) + pos.y;

            tTypes[t] = GRASS;
            tMasks[t] = 0;
            rtPositions[t] = {tPosX, tPosY, 0.0f};
            rtTexCoords[t] = TILE_CONFIGS[GRASS].frames[0];
            rtScales[t] = {TILE_SIZE_X, TILE_SIZE_Y};
        }
    }
}

void WorldManager::_updateRenderers() {
    spriteRenderer.setCount(rtCount);
    spriteRenderer.updatePositionData(0, rtCount, &rtPositions[0]);
    spriteRenderer.updateTexCoordData(0, rtCount, &rtTexCoords[0]);
    spriteRenderer.updateScaleData(0, rtCount, &rtScales[0]);
}

} // namespace Core