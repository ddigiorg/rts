#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <array>
#include <cstddef> // for std::byte
#include <cstdint>
#include <limits> // for std::numeric_limits
#include <vector>

namespace ECS {

// =============================================================================
// Chunk
//
// A contiguous block of memory storing component data for multiple entities. It
// is designed for efficient memory access and L1 cache locality.
//
// { 00, 00, 00, ..,   Header: 256 bytes
//   e0, e1, e2, eM,   EntityIDs (Chunk buffer data starts at first byte of e0)
//   00, 00, 00, ..,   Component 0
//   00, 00, 00, ..,   Component 1
//   00, 00, 00, ..  } Component N
//
// =============================================================================

class Chunk {
public:
    Chunk() { reset(); }
    void reset();
    void initialize(Archetype* a);

    template <typename T>
    bool hasComponent() const;
    bool isFull() const { return count >= capacity; }
    bool isEmpty() const { return count == 0; }
    void incrementVersion() { ++version; }

    // component entityID access
    const EntityID* getEntityIDArray() const;
    const EntityID& getEntityID(ChunkIdx index) const;

    // component data access
    template <typename T>
    T*       getComponentArrayData();
    template <typename T>
    const T* getComponentArrayData() const;
    template <typename T>
    T&       getComponentData(ChunkIdx index);
    template <typename T>
    const T& getComponentData(ChunkIdx index) const;
    template <typename T>
    void     setComponentData(ChunkIdx index, const T& value);

    // getters
    ChunkID    getID()        const { return id;        }
    GroupID    getGroupID()   const { return groupID;   }
    ChunkIdx   getCount()     const { return count;     }
    ChunkIdx   getCapacity()  const { return capacity;  }
    uint32_t   getVersion()   const { return version;   }
    Archetype* getArchetype() const { return archetype; }

private:
    friend class ChunkManager;

    void*       _getArrayPtr(ChunkIdx offset=0);
    const void* _getArrayPtr(ChunkIdx offset=0) const;
    void*       _getElementPtr(ChunkIdx offset=0, ChunkIdx stride=0);
    const void* _getElementPtr(ChunkIdx offset=0, ChunkIdx stride=0) const;

    template <typename T>
    T* _getComponentArray();

    // header
    ChunkID id;             // 4B unique identifier
    GroupID groupID;        // 4B unique group identifier
    Chunk* prevChunkActive; // 8B list node to prev active chunk
    Chunk* nextChunkActive; // 8B list node to next active chunk
    Chunk* prevChunkOpen;   // 8B list node to prev open chunk
    Chunk* nextChunkOpen;   // 8B list node to next open chunk
    ChunkIdx count;         // 2B num entities in this chunk
    ChunkIdx capacity;      // 2B max entities in this chunk
    uint32_t version;       // 4B structural change version
    Archetype* archetype;   // 8B pointer to parent archetype

    // header component address lookup tables
    std::array<ComponentID, COMPONENT_CAPACITY> toIdx;     // 64B
    std::array<void*, CHUNK_COMPONENT_CAPACITY> arrayPtrs; // 128B

    // entity component data buffer
    alignas(64) std::array<std::byte, CHUNK_BUFFER_SIZE> buffer; // 16KB - 256B
};

void Chunk::reset() {
    id = CHUNK_ID_NULL;
    prevChunkActive = nullptr;
    nextChunkActive = nullptr;
    prevChunkOpen   = nullptr;
    nextChunkOpen   = nullptr;
    count    = 0;
    capacity = 0;
    version  = 0;
    archetype  = nullptr;
    toIdx.fill(COMPONENT_ID_NULL);
    arrayPtrs.fill(nullptr);
    buffer.fill(std::byte(0));
}

void Chunk::initialize(Archetype* a) {
    archetype = a;
    capacity = archetype->getCapacity();

    // initialize component address lookup tables
    const std::vector<Component> components = archetype->getComponents();
    for (size_t i = 0; i < components.size(); i++) {
        const Component& c = components[i];
        toIdx[c.getID()] = i;
        arrayPtrs[i] = _getArrayPtr(c.getOffset());
    }
}

template <typename T>
bool Chunk::hasComponent() const {
    ComponentID id = getComponentID<T>();
    if (id < COMPONENT_CAPACITY)
        return arrayptrs[toIdx[]] != nullptr;
    return false;
}

const EntityID* Chunk::getEntityIDArray() const {
    return reinterpret_cast<const EntityID*>(_getArrayPtr());
}

const EntityID& Chunk::getEntityID(ChunkIdx index) const {
    ASSERT(index < count, "Index out of bounds.");
    return getEntityIDArray()[index];
}

template <typename T>
T* Chunk::getComponentArrayData() {
    return _getComponentArray<T>();
}

template <typename T>
const T* Chunk::getComponentArrayData() const {
    return _getComponentArray<T>();
}

template <typename T>
T& Chunk::getComponentData(ChunkIdx index) {
    ASSERT(index < count, "Index out of bounds.");
    return _getComponentArray<T>()[index];
}

template <typename T>
const T& Chunk::getComponentData(ChunkIdx index) const {
    ASSERT(index < count, "Index out of bounds.");
    return _getComponentArray<T>()[index];
}

template <typename T>
void Chunk::setComponentData(ChunkIdx index, const T& value) {
    getElement<T>(index) = value;
}

void* Chunk::_getArrayPtr(ChunkIdx offset) {
    return static_cast<void*>(buffer.data() + offset);
}

const void* Chunk::_getArrayPtr(ChunkIdx offset) const {
    return static_cast<const void*>(buffer.data() + offset);
}

void* Chunk::_getElementPtr(ChunkIdx offset, ChunkIdx stride) {
    return static_cast<void*>(buffer.data() + offset + stride);
}

const void* Chunk::_getElementPtr(ChunkIdx offset, ChunkIdx stride) const {
    return static_cast<const void*>(buffer.data() + offset + stride);
}

template <typename T>
T* Chunk::_getComponentArray() {
    ComponentID id = getComponentID<T>();
    ASSERT(hasComponent(id), "ComponentID is not in Chunk.");
    size_t offset = arrayptrs[toIdx[id]];
    ASSERT(offset + sizeof(T) * count <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return reinterpret_cast<T*>(_getArrayPtr(offset));
}

} // namespace ECS