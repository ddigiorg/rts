#pragma once

#include "ecs/types.hpp"
#include "ecs/component.hpp"
#include "utils/assert.hpp"

#include <array>
#include <vector>

namespace ECS {

class Archetype {
public:
    Archetype(
        ArchetypeID id_,
        ArchetypeMask mask_,
        const std::vector<Component>& components_
    );

    // queries
    bool hasComponent(ComponentID cID) const;

    // getters
    ArchetypeID getID() const { return id; }
    ArchetypeMask getMask() const { return mask; }
    ChunkIdx getCapacity() const { return capacity; }
    size_t getComponentCount() const { return components.size(); }
    const std::vector<Component>& getComponents() const { return components; }

private:
    // archetype metadata
	ArchetypeID id;     // unique archetype identifier
    ArchetypeMask mask; // bitmask where set bits represent components
    ChunkIdx capacity;  // maximum number of entities in a chunk of this archetype

    // chunk component metadata
    std::vector<Component> components;

    // archetype neighbors
    std::array<Archetype*, COMPONENT_CAPACITY> leftArchetypes;
    std::array<Archetype*, COMPONENT_CAPACITY> rightArchetypes;
};

Archetype::Archetype(
        ArchetypeID id_,
        ArchetypeMask mask_,
        const std::vector<Component>& components_)
{
    ASSERT(components.size() < CHUNK_COMPONENT_CAPACITY,
           "Archetype cannot contain more than 16 components.");

    id = id_;
    mask = mask_;
    components = std::move(components_);
    capacity = CHUNK_BUFFER_SIZE;

    // calculate the size of all components for a single entity
    ComponentSize entitySize = sizeof(EntityID); // NOTE: first component is always EntityID
    for (const Component& c : components) {
        entitySize += c.getSize();
    }
    ASSERT(entitySize < CHUNK_BUFFER_SIZE,
           "Archetype component data size exceeds chunk buffer size.");
    if (entitySize > 0) {
        capacity = CHUNK_BUFFER_SIZE / entitySize;
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