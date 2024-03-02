#pragma once

#include "types.hpp"

#include <cassert> // assert

namespace ECS {

class Chunk {
public:
    friend class Archetype;

    // 16 kilobytes which good size for most CPU L1 caches
    static const size_t SIZE = 1024 * 16;
    static const size_t OVERHEAD = 256; // bytes
    static const size_t BUFFER_SIZE = SIZE - OVERHEAD;

    template<typename T>
    void Set(const size_t index, T& value);
    template<typename T>
    T* Get(const size_t index);
    template<typename T>
    T* GetData();
    void* GetBufferLocation(const ComponentID cid, const size_t index);
    size_t GetCount() { return count; };
    World* GetWorld() { return world; };
    Archetype* GetArchetype() { return arch; };

private:
    using byte = char;

    World* world;
    Archetype* arch;
    size_t chunkArrayIndex;
    size_t count;
    byte bufferAnchor;
};

template<typename T>
void Chunk::Set(const size_t index, T& value) {
    ComponentID cid = GetComponentID<T>();
    assert(index < arch->GetCapacity());
    assert(arch->HasComponent(cid));
    *reinterpret_cast<T*>(GetBufferLocation(cid, index)) = value;
}

template<typename T>
T* Chunk::Get(const size_t index) {
    ComponentID cid = GetComponentID<T>();
    assert(index < arch->GetCapacity());
    assert(arch->HasComponent(cid));
    return reinterpret_cast<T*>(GetBufferLocation(cid, index));
}

template<typename T>
T* Chunk::GetData() {
    ComponentID cid = GetComponentID<T>();
    assert(arch->HasComponent(cid));
    return reinterpret_cast<T*>(GetBufferLocation(cid, 0));
}

} // namespace ECS