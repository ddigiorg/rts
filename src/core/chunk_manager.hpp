#pragma once

#include "common/types.hpp"

#include <unordered_map>
#include <array>
#include <vector>

// struct Sector {
//     std::vector<EntityID> eIds;
//     std::vector<BoundingBox> eBoxes;
// };

struct Chunk {
    ChunkHash hash;
    ChunkCell cell; // integer grid coordinate, perhaps rename to GridPosition
    // std::array<Chunk*, 8> neighbors;
    // std::array<Sector, CHUNK_NUM_SECTORS> collisionSectors;
    // std::array<Sector, CHUNK_NUM_SECTORS> selectionSectors;
    std::vector<EntityID> eIds;
    std::vector<BoundingBox> eRenderXYBoxes;
    std::vector<Vec4<float>> eRenderColors;
};

class ChunkManager {
public:
    ChunkManager();

    // void loadChunk(ChunkCell cell); // TODO: read chunk from file and insert runtime data
    // void unloadChunk(ChunkHash hash); // TODO: write chunk to file and remove runtime data
    // void activateChunk();
    // void deactivateChunk();

    // void onEntityCreated();
    // void onEntityRemoved();
    // void onEntityMoved();

    // Chunk* getChunkAtWorldPos(const Vec2<float>& worldPos); // TODO: maybe WorldPosition should be a type
    // Sector* getSectorAtWorldPos(const Vec2<float>& worldPos);

private:
    std::unordered_map<ChunkHash, Chunk, ChunkHashFunctor, ChunkEqualFunctor> chunks;
};

ChunkManager::ChunkManager() {

}