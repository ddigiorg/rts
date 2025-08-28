#pragma once

#include "ecs/types.hpp"
#include "ecs/chunk.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"

#include <array>
#include <deque>
#include <vector>
#include <unordered_map>

namespace ECS {

class Archetype {
public:
    Archetype(id_t id_, mask_t mask_, const std::vector<Component>& components_);

    bool hasComponent(id_t id) const { return (mask & (mask_t(1) << id)) != 0; }

    void insertEntity(Entity& e);
    // void removeEntity(Entity& e);
    // void moveEntityRight(Entity& entity, id_t componentId);
    // void moveEntityLeft(Entity& entity, id_t componentId);

    id_t getId() const { return id; }
    mask_t getMask() const { return mask; }
    uint32_t getCapacity() const { return capacity; }
    size_t getChunkCount() const { return chunks.size(); }
    // std::vector<Chunk*>& getChunks() { return chunks; }
    const std::vector<Component>& getComponents() const { return components; }
    size_t getArrayOffset(id_t id) const;

private:
    void _pushChunk();
    void _popChunk();

    id_t   id;         // unique archetype identifier
    mask_t mask;       // bitmask set bits representing components
    uint32_t capacity; // chunk entity capacity

    std::deque<Chunk> chunks;          // all chunks of this archetype
    std::vector<Chunk*> openChunks;    // chunks with available slots
    std::vector<Component> components; // archetype components

    std::array<size_t, Component::CAPACITY> offsets;
    std::array<id_t,   Component::CAPACITY> leftArchetypes;
    std::array<id_t,   Component::CAPACITY> rightArchetypes;
};

Archetype::Archetype(id_t id_, mask_t mask_, const std::vector<Component>& components_) {
    id = id_;
    mask = mask_;
    capacity = Chunk::BUFFER_SIZE;
    components = components_;
    offsets.fill(SIZE_MAX);
    leftArchetypes.fill(INVALID_ID);
    rightArchetypes.fill(INVALID_ID);

    // calculate the size of all components for a single entity
    size_t entitySize = sizeof(id_t); // NOTE: first chunk component is always entityIds
    for (const Component& component : components) {
        entitySize += component.size;
    }
    ASSERT(entitySize < Chunk::BUFFER_SIZE, "Archetype component data size exceeds chunk buffer size.");
    if (entitySize > 0) {
        capacity = Chunk::BUFFER_SIZE / entitySize;
    }

    // assign component information
    size_t offset = sizeof(id_t) * capacity;
    for (const Component& component : components) {
        offsets[component.id] = offset;
        offset += component.size * capacity;
    }
}

size_t Archetype::getArrayOffset(id_t id) const {
    ASSERT(hasComponent(id), "Archetype does not contain component " << id << ".");
    return offsets[id];
}

void Archetype::insertEntity(Entity& e) {

    // if no open chunks then allocate a new chunk
    if (openChunks.empty()) {
        _pushChunk();
    }

    Chunk* chunk = openChunks.back();

    // assign entity location
    e.chunk = chunk;
    e.index = chunk->entityCount++;

    // set entity id in chunk
    size_t eOffset = e.index * sizeof(id_t);
    void* elementPtr = chunk->_getBuffer(eOffset);
    std::memset(elementPtr, e.id, sizeof(id_t));

    // initialize component data for this entity
    for (const Component& component : components) {
        size_t cOffset = offsets[component.id];
        size_t eOffset = e.index * component.size;
        void* elementPtr = chunk->_getBuffer(cOffset + eOffset);
        std::memset(elementPtr, 0, component.size);
    }

    // if chunk just became full remove it from the open list
    if (chunk->isFull()) {
        openChunks.pop_back();
    }
}

// void Archetype::removeEntity(Entity& e) {
//     Chunk* chunk = chunks[e.index];

//     // TODO: swap and pop
//     for (const Component& component : components) {

//     }

//     e.chunk = nullptr;
//     e.index = 0;

//     if (!chunk->isFull() && 
//         std::find(openChunks.begin(), openChunks.end(), chunk) == openChunks.end()) {
//         openChunks.push_back(chunk);
//     }
// }

void Archetype::_pushChunk() {
    chunks.emplace_back(this, capacity);
    openChunks.push_back(&chunks.back());
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