#pragma once

#include "ecs/types.hpp"
#include "ecs/component.hpp"
#include "utils/assert.hpp"

#include <array>
#include <vector>

namespace ECS {

// =============================================================================
// Archetype
// =============================================================================
    
class Archetype {
    friend class ArchetypeManager;

public:
    Archetype(
        ArchetypeID id_,
        ArchetypeMask mask_,
        const std::vector<Component>& components_);

    bool hasComponent(ComponentID cID) const;

    ArchetypeID getID() const { return id; }
    ArchetypeMask getMask() const { return mask; }
    ChunkIdx getCapacity() const { return capacity; }
    const std::vector<Component>& getComponents() const { return components; }

private:
	ArchetypeID id;     // unique archetype identifier
    ArchetypeMask mask; // bitmask where set bits represent components
    ChunkIdx capacity;  // maximum number of entities in chunk of this archetype

    std::vector<Component> components;

    // neighbor graph nodes
    std::array<Archetype*, COMPONENT_CAPACITY> removes;
    std::array<Archetype*, COMPONENT_CAPACITY> inserts;
};

// =============================================================================
// Archetype Functions
// =============================================================================

Archetype::Archetype(
        ArchetypeID id_,
        ArchetypeMask mask_,
        const std::vector<Component>& components_ ) {

    ASSERT(components.size() < CHUNK_COMPONENT_CAPACITY,
        "Archetype cannot contain more than 16 components.");

    id = id_;
    mask = mask_;
    components = std::move(components_);
    capacity = CHUNK_BUFFER_SIZE;

    // get size of all component data in a single entity of this archetype
    // NOTE: first "component" is always EntityID
    ChunkIdx eSize = static_cast<ChunkIdx>(sizeof(EntityID));
    for (const Component& c : components) {
        eSize += c.getSize();
    }

    ASSERT(eSize < CHUNK_BUFFER_SIZE,
        "Archetype component data size exceeds chunk buffer size.");

    // set archetype capacity (i.e. num entities in a chunk) based on data size
    if (eSize > 0) {
        capacity = CHUNK_BUFFER_SIZE / eSize;
    }

    // assign component information
    ComponentSize offset = sizeof(EntityID) * capacity;
    for (Component& c : components) {
        c._setOffset(offset);
        offset += c.getSize() * capacity;
    }
}

bool Archetype::hasComponent(ComponentID cID) const { 
    return (mask & (ArchetypeMask(1) << cID)) != 0;
}

} // namespace ECS