#pragma once

#include "ecs/types.hpp"
#include "utils/assert.hpp"

#include <array>
#include <cstddef> // for std::byte
#include <cstdint>

namespace ECS {

// A Chunk represents a contiguous block of memory for storing entity component 
// data.  It is designed for efficient memory access and L1 cache locality.

class Chunk {
public:
    Chunk(); // TODO

    // TODO
    uint32_t addEntity(); // returns new entity index
    void removeEntity(uint32_t index); // swap-pop for dense arrays
    void copyEntityTo(uint32_t index, Chunk& dst, uint32_t dstIndex);

    template <typename C>
    bool hasComponent() const;

    template <typename C>
    C* getArray();

    template <typename C>
    const C* getArray() const;

    template <typename C>
    C& getElement(uint32_t index);

    template <typename C>
    const C& getElement(uint32_t index) const;

    template <typename C>
    void setElement(uint32_t index, const C& value);

    bool isFull() const { return entityCount >= entityCapacity; }

    void incrementVersion() { ++version; }
    size_t getEntityCount() const { return entityCount; }
    size_t getCapacity() const { return entityCapacity; }
    Archetype* getArchetype() const { return archetype; }

    static constexpr size_t TOTAL_SIZE  = 16 * 1024;
    static constexpr size_t HEADER_SIZE = 128;
    static constexpr size_t BUFFER_SIZE = TOTAL_SIZE - HEADER_SIZE;

private:
    // --- Header (metadata, 256 bytes) ---
    uint32_t entityCount;       // num entities currently in this chunk
    uint32_t entityCapacity;    // max entities for this chunk
    uint32_t version;           // structural change version
    Archetype* archetype;       // pointer to parent archetype
    void* sharedComponentArray; // pointer to shared component data

    // --- Data buffer (aligned packed storage, 16 KB - 256 B) ---
    alignas(64) std::array<std::byte, BUFFER_SIZE> buffer;
};

template <typename C>
bool Chunk::hasComponent() const {
    return archetype->hasComponent(getComponentId<C>());
}

template <typename C>
C* Chunk::getArray() {
    ASSERT(archetype != nullptr, "Chunk has no archetype set.");
    size_t offset = archetype->getArrayOffset(getComponentId<C>());
    ASSERT(offset + sizeof(C) * entityCount <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return reinterpret_cast<C*>(buffer.data() + offset);
}

template <typename C>
const C* Chunk::getArray() const {
    ASSERT(archetype != nullptr, "Chunk has no archetype set.");
    size_t offset = archetype->getArrayOffset(getComponentId<C>());
    ASSERT(offset + sizeof(C) * entityCount <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return reinterpret_cast<const C*>(buffer.data() + offset);
}

template <typename C>
C& Chunk::getElement(uint32_t index) {
    ASSERT(index < entityCount, "Index out of bounds.");
    return getArray<C>()[index];
}

template <typename C>
const C& Chunk::getElement(uint32_t index) const {
    ASSERT(index < entityCount, "Index out of bounds.");
    return getArray<C>()[index];
}

template <typename C>
void setElement(uint32_t index, const C& value) {
    ASSERT(index < entityCount, "Index out of bounds.");
    getArray<C>()[index] = value;
}

} // namespace ECS