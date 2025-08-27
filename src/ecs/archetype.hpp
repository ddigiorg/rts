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
    Archetype(id_t id_, mask_t mask_, const std::vector<Component*>& components_);
    ~Archetype();

    bool hasComponent(id_t id) const { return (mask & (mask_t(1) << id)) != 0; }

    void insertEntity(Entity& entity);
    // void removeEntity(Entity& entity);
    // void moveEntityRight(Entity& entity, id_t componentId);
    // void moveEntityLeft(Entity& entity, id_t componentId);

    id_t getId() const { return id; }
    mask_t getMask() const { return mask; }
    idx_t getChunkOpenIdx() const { return chunkOpenIdx; }
    uint32_t getCapacity() const { return capacity; }
    size_t getChunkCount() const { return chunks.size(); }
    std::vector<Chunk*>& getChunks() { return chunks; }
    std::vector<Component*> getComponents() const { return components; };
    size_t getArrayOffset(id_t id) const;

private:
    void _pushChunk();
    void _popChunk();

    id_t   id;           // unique archetype identifier
    mask_t mask;         // bitmask set bits representing components
    uint32_t capacity;   // chunk entity capacity
    idx_t  chunkOpenIdx; // open chunk available for new entities

    std::vector<Chunk*> chunks;
    std::vector<Component*> components;

    std::array<size_t, Component::CAPACITY> offsets;
    std::array<id_t,   Component::CAPACITY> leftArchetypes;
    std::array<id_t,   Component::CAPACITY> rightArchetypes;
};

Archetype::Archetype(id_t id_, mask_t mask_, const std::vector<Component*>& components_) {
    id = id_;
    mask = mask_;
    capacity = Chunk::BUFFER_SIZE;
    chunkOpenIdx = 0;
    chunks.reserve(1);
    components = components_;
    offsets.fill(SIZE_MAX);
    leftArchetypes.fill(INVALID_ID);
    rightArchetypes.fill(INVALID_ID);

    // calculate the size of all components for a single entity
    size_t entitySize = sizeof(id_t); // NOTE: first chunk component is always entityIds
    for (Component* component : components) {
        entitySize += component->getSize();
    }
    ASSERT(entitySize < Chunk::BUFFER_SIZE, "Archetype component data size exceeds chunk buffer size.");
    if (entitySize > 0) {
        capacity = Chunk::BUFFER_SIZE / entitySize;
    }

    // assign component offsets
    size_t offset = sizeof(id_t) * capacity;
    for (size_t c = 0; c < components.size(); c++) {
        Component* component = components[c];
        offsets[component->getId()] = offset;
        offset += component->getSize() * capacity;
    }
}

Archetype::~Archetype() {
    for (Chunk* chunk : chunks) {
        if (chunk != nullptr) free(chunk);
    }
}

size_t Archetype::getArrayOffset(id_t id) const {
    ASSERT(hasComponent(id), "Archetype does not contain component " << id << ".");
    return offsets[id];
}

void Archetype::insertEntity(Entity& entity) {

    // allocate new chunk if no chunks exist in archetype
    if (chunks.empty()) {
        _pushChunk(); // TODO: implement
    }

    // allocate new chunk if back chunk is full
    if (chunks.back()->isFull()) {
        chunkOpenIdx = chunks.size();
        _pushChunk();
    }

    // TODO: detect open chunk and add to it
}

void Archetype::_pushChunk() {

}

void Archetype::_popChunk() {

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