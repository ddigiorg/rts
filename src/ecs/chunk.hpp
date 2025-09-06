#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <array>
#include <cstddef> // for std::byte
#include <cstdint>
#include <vector>

namespace ECS {

// =============================================================================
// Chunk
//
// A Chunk is a flat contiguous block of memory storing entity component data.
// It is designed for efficient memory access and cache locality.
//
// { h_0, h_1, ..., h_N,   header 256 bytes
//   e_0, e_1, ..., e_X,   chunk.data<EntityIDs>()
//   cA0, cA1, ..., cAX,   chunk.data<ComponentA>()
//   cB0, cB1, ..., cBX,   chunk.data<ComponentB>()
//   ..., ..., ..., ...,   ...
//   cY0, cY1, ..., cYX, } chunk.data<ComponentY>
//
// =============================================================================

class Chunk {
    friend class ChunkManager;

public:
    Chunk() { _clear(); }

    // queries
    template <typename T>
    bool hasComponent() const;
    bool isFull()  const { return count >= capacity; }
    bool isEmpty() const { return count == 0; }

    // getters
    ChunkID  getID()       const { return id;       }
    GroupID  getGroupID()  const { return groupID;  }
    ChunkIdx getCount()    const { return count;    }
    ChunkIdx getCapacity() const { return capacity; }

    // buffer access (NOTE: unsafe! can be indexed out of bounds...)
    const EntityID* getEntityIDs() const;
    template <typename T>
    T*       data();
    template <typename T>
    const T* data() const;

private:
    void _clear();
    void _initialize(ChunkID id, Archetype* a, GroupID g);
    EntityID* _getEntityIDs();

    // header (metadata)
    ChunkID id;           // 4B unique identifier
    GroupID groupID;      // 4B unique group identifier
    Chunk* prevChunk;     // 8B list node to prev chunk
    Chunk* nextChunk;     // 8B list node to next chunk
    Chunk* prevChunkOpen; // 8B list node to prev chunk with open entity slots
    Chunk* nextChunkOpen; // 8B list node to next chunk with open entity slots
    ChunkIdx count;       // 2B num entities in this chunk
    ChunkIdx capacity;    // 2B max entities in this chunk
    uint32_t version;     // 4B structural change version
    Archetype* archetype; // 8B pointer to parent archetype

    // header (component address lookup tables)
    std::array<ComponentID, COMPONENT_CAPACITY> toIdx;   // 64B
    std::array<void*, CHUNK_COMPONENT_CAPACITY> bufPtrs; // 128B

    // entity component data buffer
    alignas(64) std::array<std::byte, CHUNK_BUFFER_SIZE> buffer; // 16KB - 256B
};

// =============================================================================
// Chunk Functions
// =============================================================================

template <typename T>
bool Chunk::hasComponent() const {
    ComponentID cID = getComponentID<T>();
    if (cID < COMPONENT_CAPACITY)
        return bufPtrs[toIdx[cID]] != nullptr;
    return false;
}

EntityID* Chunk::_getEntityIDs() {
    return reinterpret_cast<EntityID*>(buffer.data());
}

const EntityID* Chunk::getEntityIDs() const {
    return reinterpret_cast<const EntityID*>(buffer.data());
}

template <typename T>
T* Chunk::data() {
    ASSERT(hasComponent<T>(), "Component is not in Chunk.");
    ComponentID cID = getComponentID<T>();
    return reinterpret_cast<T*>(bufPtrs[toIdx[cID]]);
}

template <typename T>
const T* Chunk::data() const {
    ASSERT(hasComponent<T>(), "Component is not in Chunk.");
    ComponentID cID = getComponentID<T>();
    return reinterpret_cast<const T*>(bufPtrs[toIdx[cID]]);
}

void Chunk::_clear() {
    id = CHUNK_ID_NULL;
    groupID = GROUP_ID_NULL;
    prevChunk = nullptr;
    nextChunk = nullptr;
    prevChunkOpen = nullptr;
    nextChunkOpen = nullptr;
    count    = 0;
    capacity = 0;
    version  = 0;
    archetype  = nullptr;
    toIdx.fill(COMPONENT_ID_NULL);
    bufPtrs.fill(nullptr);
    buffer.fill(std::byte(0));
}

void Chunk::_initialize(ChunkID id, Archetype* a, GroupID g) {
    this->id = id;
    archetype = a;
    groupID = g;
    capacity = archetype->getCapacity();

    // initialize component address lookup tables
    const std::vector<Component> components = archetype->getComponents();
    for (size_t i = 0; i < components.size(); i++) {
        const Component& c = components[i];
        toIdx[c.getID()] = i;
        // TODO: need to check if archetype does this
        // ASSERT(offset + sizeof(T) * count <= BUFFER_SIZE, "Component array exceeds chunk buffer.");
        bufPtrs[i] = static_cast<void*>(buffer.data() + c.getOffset());
    }
}

} // namespace ECS