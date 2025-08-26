#pragma once

#include "ecs/types.hpp"
#include "ecs/chunk.hpp"
#include "ecs/component.hpp"

#include <array>
#include <vector>
#include <unordered_map>

namespace ECS {

class Archetype {
public:
    Archetype(id_t id_, mask_t mask_, const std::vector<Component*>& components);
    ~Archetype();

    // bool hasComponent(id_t id);

    // void insertEntity(Entity& entity);
    // void removeEntity(Entity& entity);
    // void moveEntityRight(Entity& entity, id_t componentId);
    // void moveEntityLeft(Entity& entity, id_t componentId);

    id_t getId() const { return id; }
    mask_t getMask() const { return mask; }
    // size_t getOffset(id_t id);
    // std::vector<id_t> getComponentIds();
    std::vector<Chunk*>& getChunks() { return chunks; }

private:
    // void _pushChunk();
    // void _popChunk();

    id_t   id;
    mask_t mask;
    idx_t  chunkOpenIdx;
    idx_t  chunkEntityCapacity;

    std::unordered_map<id_t, size_t> idToIdx; // TODO: implement this in constructor
    std::array<size_t, Component::CAPACITY> idToOffset;
    std::array<id_t,   Component::CAPACITY> leftArchetypes;
    std::array<id_t,   Component::CAPACITY> rightArchetypes;

    std::vector<Chunk*> chunks;
};

Archetype::Archetype(id_t id_, mask_t mask_, const std::vector<Component*>& components) {
    id = id_;
    mask = mask_;
    chunkOpenIdx = 0;
    idToOffset.fill(SIZE_MAX);
    leftArchetypes.fill(INVALID_ID);
    rightArchetypes.fill(INVALID_ID);
    chunks.reserve(1);

    // gather component metadata
    size_t entitySize = sizeof(id_t); // NOTE: first chunk component is always entityIds
    for (Component* c : components) {
        entitySize += c->getSize();
    }

    ASSERT(entitySize < Chunk::BUFFER_SIZE, "Archetype component data size exceeds chunk buffer size.");

    chunkEntityCapacity = (entitySize > 0)
        ? Chunk::BUFFER_SIZE / entitySize
        : Chunk::BUFFER_SIZE;

    // assign component offsets
    size_t offset = sizeof(id_t) * chunkEntityCapacity;
    for (Component* c : components) {
        idToOffset[c->getId()] = offset;
        offset += c->getSize() * chunkEntityCapacity;
    }
}

Archetype::~Archetype() {
    for (Chunk* chunk : chunks) {
        if (chunk != nullptr) free(chunk);
    }
}

// void World::_pushChunkOnArchetype(id_t archetypeId) {
//     ASSERT(archetypeId < archetypes.size(), "Archetype " << archetypeId << " does not exist.");
//     ArchetypeRecord& aRecord = archetypes[archetypeId];

//     ChunkMeta cMeta {
//         /*   archetypeId=*/ archetypeId,
//         /*      chunkIdx=*/ (idx_t)aChunkData[archetypeId].size(),
//         /*   entityCount=*/ 0,
//         /*entityCapacity=*/ aRecord.entityCapacity,
//         /*    cidToArray=*/ {},
//     };

//     ChunkData* cData = static_cast<ChunkData*>(malloc(CHUNK_DATA_SIZE));

//     for (const auto& c : aRecord.components) {
//         if (c.componentId == INVALID_ID) continue;
//         ChunkArrayRecord& ca = cMeta.cidToArray[c.componentId];
//         ca.addr = reinterpret_cast<void*>(reinterpret_cast<std::byte*>(cData) + c.arrayOffset);
//         ca.size = c.arraySize;
//     }

//     aChunkMeta[archetypeId].push_back(cMeta);
//     aChunkData[archetypeId].push_back(cData);
// }

// void World::_popChunkOffArchetype(id_t archetypeId) {

// }

// void World::_freeChunks() {
//     for (auto& chunks : aChunkData) {
//         for (auto chunk : chunks) {
//             free(chunk);
//         }
//     }
// }

} // namespace ECS