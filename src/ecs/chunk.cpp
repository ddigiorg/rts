#include "chunk.hpp"
#include "archetype.hpp"

#include <cassert> // assert

namespace ECS {

void* Chunk::GetBufferLocation(const ComponentID cid, const size_t index) {
    assert(arch->HasComponent(cid));
    assert(index < arch->GetCapacity());
    Component& c = arch->GetComponentMap()[cid];
    return reinterpret_cast<void*>(&bufferAnchor + c.offset + c.size * index);
}

} // namespace ECS