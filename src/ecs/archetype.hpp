#pragma once

#include "ecs/types.hpp"
#include "ecs/chunk.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"

#include <algorithm> // std::find
#include <array>
#include <cstring> // for std::memset and std::memcpy
#include <vector>
#include <unordered_map>

namespace ECS {

using ArchetypeID = uint16_t;

constexpr const ArchetypeID ARCHETYPE_ID_NULL = std::numeric_limits<ArchetypeID>::max();

class Archetype {
public:
    friend class World;

    Archetype(ArchetypeID id_, mask_t mask_, const std::vector<Component>& components_);
    ~Archetype();

    bool hasComponent(ComponentID cID) const;

    void insertEntity(EntityID eID, std::vector<Entity>& entities);
    void removeEntity(EntityID eID, std::vector<Entity>& entities);
    // void moveEntityRight(Entity& entity, ComponentID cID);
    // void moveEntityLeft(Entity& entity, ComponentID cID);

    ArchetypeID getID() const { return id; }
    mask_t getMask() const { return mask; }
    uint16_t getCapacity() const { return capacity; }
    size_t getChunkCount() const { return chunks.size(); }
    // std::vector<Chunk*>& getChunks() { return chunks; }
    const std::vector<Component>& getComponents() const { return components; }
    uint16_t getArrayOffset(ComponentID cID) const;

private:
    void _allocateChunk();
    void _deactivateChunk(Chunk* chunk);

    // archetype data
    ArchetypeID id;    // unique archetype identifier
    mask_t mask;       // bitmask set bits representing components
    uint16_t capacity; // chunk entity capacity

    // chunk data
    Chunk* headActiveChunk;         // doubly-linked list of active chunks
    Chunk* tailActiveChunk;         // doubly-linked list of active chunks
    std::vector<Chunk*> chunks;     // chunk storage
    std::vector<Chunk*> openChunks; // chunks with space for new entities
    std::vector<Chunk*> freeChunks; // empty chunks (aka "inactive")

    // for (Chunk* chunk = headActiveChunk; chunk != nullptr; chunk = chunk->nextActiveChunk) {
    //     for (uint16_t i = 0; i < chunk->count; ++i) {
    //         EntityID eID = chunk->getEntityID(i);
    //         // access entity data here via chunk->getComponentArrayData<TYPE>() or similar
    //     }
    // }

    // component data
    std::vector<Component> components; // archetype components
    std::array<uint16_t, COMPONENT_CAPACITY> cOffsets; // component offsets

    // archetype neighbor data
    std::array<Archetype*, COMPONENT_CAPACITY> leftArchetypes;
    std::array<Archetype*, COMPONENT_CAPACITY> rightArchetypes;
};

Archetype::Archetype(ArchetypeID id_, mask_t mask_, const std::vector<Component>& components_) {
    id = id_;
    mask = mask_;
    capacity = Chunk::BUFFER_SIZE;

    headActiveChunk = nullptr;
    tailActiveChunk = nullptr;

    components = components_;
    cOffsets.fill(UINT16_MAX);

    leftArchetypes.fill(nullptr);
    rightArchetypes.fill(nullptr);

    // calculate the size of all components for a single entity
    uint16_t entitySize = sizeof(EntityID); // NOTE: first component is always EntityID
    for (const Component& c : components) {
        entitySize += c.size;
    }
    ASSERT(entitySize < Chunk::BUFFER_SIZE, "Archetype component data size exceeds chunk buffer size.");
    if (entitySize > 0) {
        capacity = Chunk::BUFFER_SIZE / entitySize;
    }

    // assign component information
    uint16_t cOffset = sizeof(EntityID) * capacity;
    for (const Component& c : components) {
        cOffsets[c.id] = cOffset;
        cOffset += c.size * capacity;
    }
}

Archetype::~Archetype() {
    for (Chunk* c : chunks)
        delete c;
}

bool Archetype::hasComponent(ComponentID cID) const { 
    return (mask & (mask_t(1) << cID)) != 0;
}

void Archetype::insertEntity(EntityID eID, std::vector<Entity>& entities) {

    // if no open chunks then allocate a new chunk
    if (openChunks.empty())
        _allocateChunk();

    Chunk* chunk = openChunks.back();
    ASSERT(chunk != nullptr, "openChunks.back() returned null");

    // assign entity location
    Entity& entity = entities[eID];
    entity.chunk = chunk;
    entity.index = chunk->count++;

    // set entity id component in chunk
    void* dst = chunk->_getElementPtr(0, entity.index * sizeof(EntityID));
    std::memcpy(dst, &entity.id, sizeof(EntityID));

    // set entity data components in chunk
    for (const Component& c : components) {
        void* dst = chunk->_getElementPtr(cOffsets[c.id], entity.index * c.size);
        std::memset(dst, 0, c.size);
    }

    // if chunk is full then remove from open list
    if (chunk->isFull())
        openChunks.pop_back();
}

void Archetype::removeEntity(EntityID eID, std::vector<Entity>& entities) {
    Entity& remvEntity = entities[eID];
    Chunk* chunk = remvEntity.chunk;
    uint16_t remvIndex = remvEntity.index;
    uint16_t lastIndex = chunk->count - 1;

    if (remvIndex != lastIndex) {
        EntityID lastID = chunk->getEntityID(lastIndex);
        Entity& lastEntity = entities[lastID];

        // swap entity id component in chunk
        void* dst = chunk->_getElementPtr(0, remvIndex * sizeof(EntityID));
        void* src = chunk->_getElementPtr(0, lastIndex * sizeof(EntityID));
        std::memcpy(dst, src, sizeof(EntityID));

        // swap entity data components in chunk
        for (const Component& c : components) {
            uint16_t cOffset = cOffsets[c.id];
            void* dst = chunk->_getElementPtr(cOffset, remvIndex * c.size);
            void* src = chunk->_getElementPtr(cOffset, lastIndex * c.size);
            std::memcpy(dst, src, c.size);
        }

        // reassign swapped entity location
        lastEntity.index = remvIndex;
    }

    // reduce chunk count
    chunk->count--;

    // if chunk became not full then ensure it's in openChunks
    if (!chunk->isFull() && 
        std::find(openChunks.begin(), openChunks.end(), chunk) == openChunks.end()) {
        openChunks.push_back(chunk);
    }

    // if chunk is completely empty then move it to freeChunks
    if (chunk->isEmpty()) {
        _deactivateChunk(chunk);
    }

    ASSERT(chunk->count <= capacity, "Chunk overfilled/underflow");
}

uint16_t Archetype::getArrayOffset(ComponentID id) const {
    ASSERT(hasComponent(id), "Archetype does not contain component " << id << ".");
    return cOffsets[id];
}

void Archetype::_allocateChunk() {
    Chunk* chunk = nullptr;

    if (!freeChunks.empty()) {
        chunk = freeChunks.back();
        freeChunks.pop_back();
        chunk->init(this, capacity);
    } else {
        chunk = new Chunk(this, capacity);
        chunks.push_back(chunk);
    }
    
    ASSERT(chunk->archetype == this, "chunk->archetype must point back to its parent archetype");
    openChunks.push_back(chunk);

    // insert into active list
    chunk->prevActiveChunk = tailActiveChunk;
    chunk->nextActiveChunk = nullptr;
    if (tailActiveChunk)
        tailActiveChunk->nextActiveChunk = chunk;
    tailActiveChunk = chunk;
    if (!headActiveChunk)
        headActiveChunk = chunk;
}

void Archetype::_deactivateChunk(Chunk* chunk) {

    // remove chunk from linked list
    if (chunk->prevActiveChunk)
        chunk->prevActiveChunk->nextActiveChunk = chunk->nextActiveChunk;
    if (chunk->nextActiveChunk)
        chunk->nextActiveChunk->prevActiveChunk = chunk->prevActiveChunk;
    if (chunk == headActiveChunk)
        headActiveChunk = chunk->nextActiveChunk;
    if (chunk == tailActiveChunk)
        tailActiveChunk = chunk->prevActiveChunk;
    chunk->nextActiveChunk = nullptr;
    chunk->prevActiveChunk = nullptr;

    // remove from openChunks if present
    auto it = std::find(openChunks.begin(), openChunks.end(), chunk);
    if (it != openChunks.end()) {
        openChunks.erase(it);
    }

    freeChunks.push_back(chunk);
}

} // namespace ECS