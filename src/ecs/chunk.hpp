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
    friend class Archetype;

    Chunk(Archetype* a, uint32_t capacity);

    template <typename T>
    bool hasComponent() const;
    bool isFull() const { return entityCount >= entityCapacity; }
    void incrementVersion() { ++version; }

    // component array access
    template <typename T>
    T* getArray();
    template <typename T>
    const T* getArray();

    // component element access
    template <typename T>
    T& getElement(uint32_t index);
    template <typename T>
    const T& getElement(uint32_t index) const;
    template <typename T>
    void setElement(uint32_t index, const T& value);

    // getters
    uint32_t getCount() const { return entityCount; }
    uint32_t getCapacity() const { return entityCapacity; }
    uint32_t getVersion() const { return version; }
    Archetype* getArchetype() const { return archetype; }

    static constexpr size_t TOTAL_SIZE  = 16 * 1024;
    static constexpr size_t HEADER_SIZE = 128;
    static constexpr size_t BUFFER_SIZE = TOTAL_SIZE - HEADER_SIZE;

private:
    void* _getBuffer(size_t offset = 0);
    const void* _getBuffer(size_t offset = 0) const;
    template <typename T>
    T* _getBufferAs(size_t offset = 0);
    template <typename T>
    const T* _getBufferAs(size_t offset = 0) const;
    template <typename T>
    T* _getArray();

    // --- Header (metadata, 256 bytes) ---
    uint32_t entityCount;       // num entities currently in this chunk
    uint32_t entityCapacity;    // max entities for this chunk
    uint32_t version;           // structural change version
    Archetype* archetype;       // pointer to parent archetype
    void* sharedComponentArray; // pointer to shared component data

    // --- Data buffer (aligned packed storage, 16 KB - 256 B) ---
    alignas(64) std::array<std::byte, BUFFER_SIZE> buffer;
};

Chunk::Chunk(Archetype* a, uint32_t capacity) {
    entityCount = 0;
    entityCapacity = capacity;
    version = 0;
    archetype = a;
    sharedComponentArray = nullptr;
}

template <typename T>
bool Chunk::hasComponent() const {
    ASSERT(archetype != nullptr, "Chunk has no archetype set.");
    return archetype->hasComponent(getComponentId<T>());
}

template <typename T>
T* Chunk::getArray() {
    return _getArray<T>();
}

template <typename T>
const T* Chunk::getArray() {
    return _getArray<T>();
}

template <typename T>
T& Chunk::getElement(uint32_t index) {
    ASSERT(index < entityCount, "Index out of bounds.");
    return _getArray<T>()[index];
}

template <typename T>
const T& Chunk::getElement(uint32_t index) const {
    ASSERT(index < entityCount, "Index out of bounds.");
    return _getArray<T>()[index];
}

template <typename T>
void Chunk::setElement(uint32_t index, const T& value) {
    getElement<T>(index) = value;
}

void* Chunk::_getBuffer(size_t offset) {
    return static_cast<void*>(buffer.data() + offset);
}

const void* Chunk::_getBuffer(size_t offset) const {
    return static_cast<const void*>(buffer.data() + offset);
}

template <typename T>
T* Chunk::_getBufferAs(size_t offset) {
    return reinterpret_cast<T*>(buffer.data() + offset);
}

template <typename T>
const T* Chunk::_getBufferAs(size_t offset) const {
    return reinterpret_cast<const T*>(buffer.data() + offset);
}

template <typename T>
T* Chunk::_getArray() {
    ASSERT(archetype, "Chunk has no archetype set.");
    size_t offset = archetype->getArrayOffset(getComponentId<T>());
    ASSERT(offset + sizeof(T) * entityCount <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return _getBufferAs<T>(offset);
}

} // namespace ECS