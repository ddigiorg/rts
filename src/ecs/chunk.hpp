#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <array>
#include <deque>
#include <cstddef> // for std::byte
#include <cstdint>
#include <unordered_map>
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

    Chunk(Archetype* archetype_, uint16_t capacity_);
    void init(Archetype* archetype_, uint16_t capacity_);

    template <typename T>
    bool hasComponent() const;
    bool isFull() const { return count >= capacity; }
    bool isEmpty() const { return count == 0; }
    void incrementVersion() { ++version; }

    // component entityID access
    const EntityID* getEntityIDArray() const;
    const EntityID& getEntityID(uint16_t index) const;

    // component data access
    template <typename T>
    T*       getComponentArrayData();
    template <typename T>
    const T* getComponentArrayData() const;
    template <typename T>
    T&       getComponentData(uint16_t index);
    template <typename T>
    const T& getComponentData(uint16_t index) const;
    template <typename T>
    void     setComponentData(uint16_t index, const T& value);

    // getters
    uint16_t getCount()    const { return count;    }
    uint16_t getCapacity() const { return capacity; }
    uint32_t getVersion()  const { return version;  }
    Archetype* getArchetype() const { return archetype; }

private:
    void*       _getArrayPtr(uint16_t offset=0);
    const void* _getArrayPtr(uint16_t offset=0) const;
    void*       _getElementPtr(uint16_t offset=0, uint16_t stride=0);
    const void* _getElementPtr(uint16_t offset=0, uint16_t stride=0) const;

    template <typename T>
    T* _getComponentArray();

    // --- header ---
    Chunk* prevActiveChunk;     // 8B list node to prev active chunk
    Chunk* nextActiveChunk;     // 8B list node to next active chunk
    uint16_t count;             // 2B num entities in this chunk
    uint16_t capacity;          // 2B max entities in this chunk
    uint32_t version;           // 4B structural change version
    Archetype* archetype;       // 8B pointer to parent archetype
    void* sharedComponentArray; // 8B pointer to shared data

    // TODO: IMPLEMENT THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // lookup tables to convert cID to arrayPtr
    // arrayPtrs[toIdx[cID]]
    std::array<ComponentID, COMPONENT_CAPACITY> toIdx; // 64B
    std::array<void*, 16> arrayPtrs;                   // 128B

    // --- aligned packed entity component data ---
    std::array<std::byte, CHUNK_BUFFER_SIZE> buffer; // 16KB - 256B

    // TODO: This wont work with new and delete on MSVC
    // alignas(64) std::array<std::byte, BUFFER_SIZE> buffer;
};

Chunk::Chunk(Archetype* archetype_, uint16_t capacity_) {
    init(archetype_, capacity_);
}

void Chunk::init(Archetype* archetype_, uint16_t capacity_) {
    prevActiveChunk = nullptr;
    nextActiveChunk = nullptr;
    count = 0;
    capacity = capacity_;
    version = 0;
    archetype = archetype_;
    sharedComponentArray = nullptr;
    buffer.fill(std::byte(0));
}

template <typename T>
bool Chunk::hasComponent() const {
    ASSERT(archetype != nullptr, "Chunk has no archetype set.");
    return archetype->hasComponent(getComponentID<T>());
}

const EntityID* Chunk::getEntityIDArray() const {
    return reinterpret_cast<const EntityID*>(_getArrayPtr());
}

const EntityID& Chunk::getEntityID(uint16_t index) const {
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
T& Chunk::getComponentData(uint16_t index) {
    ASSERT(index < count, "Index out of bounds.");
    return _getComponentArray<T>()[index];
}

template <typename T>
const T& Chunk::getComponentData(uint16_t index) const {
    ASSERT(index < count, "Index out of bounds.");
    return _getComponentArray<T>()[index];
}

template <typename T>
void Chunk::setComponentData(uint16_t index, const T& value) {
    getElement<T>(index) = value;
}

void* Chunk::_getArrayPtr(uint16_t offset) {
    return static_cast<void*>(buffer.data() + offset);
}

const void* Chunk::_getArrayPtr(uint16_t offset) const {
    return static_cast<const void*>(buffer.data() + offset);
}

void* Chunk::_getElementPtr(uint16_t offset, uint16_t stride) {
    return static_cast<void*>(buffer.data() + offset + stride);
}

const void* Chunk::_getElementPtr(uint16_t offset, uint16_t stride) const {
    return static_cast<const void*>(buffer.data() + offset + stride);
}

template <typename T>
T* Chunk::_getComponentArray() {
    ASSERT(archetype, "Chunk has no archetype set.");
    size_t offset = archetype->getArrayOffset(getComponentID<T>());
    ASSERT(offset + sizeof(T) * count <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return reinterpret_cast<T*>(_getArrayPtr(offset));
}

// =============================================================================
// Chunk Manager
// =============================================================================

class ChunkManager {
public:
    // ChunkManager(std::vector<Entity>& entities_);
    // void insertEntity();

private:
    // void _getOrCreateArchetype();
    // void _allocateChunk();
    // void _deactivateChunk(Chunk* chunk);

    // ArchetypeManager& archetypeMgr;
    // EntityManager& entityMgr;

    //  chunk data
    std::deque<Chunk> chunks;         // chunk storage
    std::deque<ChunkList> chunkLists; // chunk list storage
	std::vector<Chunk*> freeChunks;   // empty chunks recycled for later reuse

    // TODO: put in component manager?
    // chunk shared component data
	// std::deque<void*> chunkSharedData; // indices are SharedComponentID
    // std::vector<SharedComponentID> freeSharedIDs;

    std::deque<std::unordered_map<ArchetypeMask, uint32_t>> maskToIndex;

};

} // namespace ECS