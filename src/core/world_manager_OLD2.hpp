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
    void render(const Camera& camera);

private:
    void _initChunkData(const size_t cIdx, const Location& cLoc);

    const Scale cScale = Scale{CHUNK_SIZE_X, CHUNK_SIZE_Y};
    const Scale tScale = Scale{TILE_SIZE_X, TILE_SIZE_Y};
    const Color cGridColor = Color{0.0f, 0.0f, 1.0f, 1.0f};
    const Color tGridColor = Color{0.0f, 0.0f, 0.0f, 0.4f};
    const float cLineWidth = 3.0f;
    const float tLineWidth = 1.0f;

    // renderers
    SpriteRenderer tSpriteRenderer;
    QuadRenderer tGridRenderer;
    QuadRenderer cGridRenderer;

    // chunk data
    size_t cCount;
    std::vector<Position> cPositions;
    std::vector<size_t> cTileIndices;

    // tile data
    size_t tCount;
    std::vector<Position> tPositions;
    std::vector<TexCoord> tTexCoords;
};

WorldManager::WorldManager() {

    // setup chunk data
    size_t cCountX = CHUNK_RENDER_RADIUS * 2 - 1;
    size_t cCountY = CHUNK_RENDER_RADIUS * 2 - 1;
    size_t cCount = cCountX * cCountY;

    cPositions.resize(cCount);
    cTileIndices.resize(cCount);

    cGridRenderer.setCapacity(cCount);
    cGridRenderer.setCount(cCount);

    // setup tile data
    size_t tCountX = cCountX * CHUNK_TILE_COUNT_X;
    size_t tCountY = cCountY * CHUNK_TILE_COUNT_Y;
    size_t tCount = tCountX * tCountY;

    tPositions.resize(tCount);
    tTexCoords.resize(tCount);

    tSpriteRenderer.setCapacity(tCount);
    tSpriteRenderer.setCount(tCount);    
    tGridRenderer.setCapacity(tCount);
    tGridRenderer.setCount(tCount);

    // initialize data
    const int cRenderHalf = CHUNK_RENDER_RADIUS - 1;
    size_t cIdx = 0;

    for (int cy = -cRenderHalf; cy <= cRenderHalf; cy++) {
        for (int cx = -cRenderHalf; cx <= cRenderHalf; cx++) {
            _initChunkData(cIdx++, Location{cx, cy});
        }
    }

    // update tile renderer buffers
    tSpriteRenderer.updatePositionData(0, tCount, &tPositions[0]);
    tSpriteRenderer.updateTexCoordData(0, tCount, &tTexCoords[0]);
    tGridRenderer.updatePositionData(0, tCount, &tPositions[0]);

    for (size_t t = 0; t < tCount; t++) {
        tSpriteRenderer.updateScaleData(t, 1, &tScale);
        tGridRenderer.updateScaleData(t, 1, &tScale);
        tGridRenderer.updateColorData(t, 1, &tGridColor);
    }

    // update chunk renderer buffers
    cGridRenderer.updatePositionData(0, cCount, &cPositions[0]);

    for (size_t c = 0; c < cCount; c++) {
        cGridRenderer.updateScaleData(c, 1, &cScale);
        cGridRenderer.updateColorData(c, 1, &cGridColor);
    }
}


void WorldManager::_initChunkData(const size_t cIdx, const Location& cLoc) {
    const size_t tIdxBeg = cIdx * CHUNK_TILE_COUNT;
    const int txBeg = CHUNK_TILE_COUNT_X * cLoc.x;
    const int tyBeg = CHUNK_TILE_COUNT_Y * cLoc.y;
    const int txEnd = txBeg + CHUNK_TILE_COUNT_X;
    const int tyEnd = tyBeg + CHUNK_TILE_COUNT_Y;

    size_t tIdx = tIdxBeg;

    for (int ty = tyBeg; ty < tyEnd; ty++) {
        for (int tx = txBeg; tx < txEnd; tx++) {
            Location tLoc = {tx, ty};
            tPositions[tIdx] = tileGridToWorld(tLoc);
            tTexCoords[tIdx] = computeTexCoord( 64, 128,  64, 64); // TODO: need tile config
            tIdx++;
        }
    }

    cPositions[cIdx] = chunkGridToWorld(cLoc);
    cTileIndices[cIdx] = tIdxBeg;
}

void WorldManager::render(const Camera& camera) {
    tSpriteRenderer.render(camera);
    tGridRenderer.renderOutline(camera, tLineWidth);
    cGridRenderer.renderOutline(camera, cLineWidth);
}

} // namespace Core