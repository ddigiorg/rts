#pragma once

#include "ecs/types.hpp"
#include "ecs/component.hpp"

#include <algorithm> /// for std::sort
#include <array>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>

namespace ECS {

// =============================================================================
// Archetype
// =============================================================================

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
        c.setOffset(offset);
        offset += c.getSize() * capacity;
    }
}

bool Archetype::hasComponent(ComponentID cID) const { 
    return (mask & (ArchetypeMask(1) << cID)) != 0;
}

// =============================================================================
// Archetype Manager
// =============================================================================

class ArchetypeManager {
public:
    ArchetypeManager(ComponentManager& componentMgr_);

    template<typename... Components>
    Archetype& getOrCreateArchetype();

    // miscellaneous
    void print();

private:
    ComponentManager& componentMgr;

    std::deque<Archetype> archetypes;
    std::unordered_map<ArchetypeMask, ArchetypeID> maskToIDs;
};

ArchetypeManager::ArchetypeManager(ComponentManager& componentMgr_)
        : componentMgr(componentMgr_), archetypes({}), maskToIDs({}) {
    getOrCreateArchetype();
}

template<typename... Components>
Archetype& ArchetypeManager::getOrCreateArchetype() {

    // build a vector of registered components
    std::vector<Component> components;
    components.reserve(sizeof...(Components));

    // helper lambda to add one component type
    auto _addComponent = [&](auto typeTag) {
        using T = decltype(typeTag);
        Component& c = componentMgr.getComponent<T>();
        components.push_back(c);
    };

    // expand pack
    (_addComponent(Components{}), ...);

    // sort components by ID
    std::sort(
        components.begin(),
        components.end(),
        [](const Component& a, const Component& b) {
            return a.getID() < b.getID();
        }
    );

    // build archetype mask
    ArchetypeMask mask = 0;
    for (const Component& c : components) {
        mask |= (ArchetypeMask(1) << c.getID());
    }

    // if archetype already exists then return it
    auto it = maskToIDs.find(mask);
    if (it != maskToIDs.end()) {
        return archetypes[it->second];
    }

    // otherwise create new archetype
    ArchetypeID id = static_cast<ArchetypeID>(archetypes.size());
    archetypes.emplace_back(id, mask, std::move(components));
    maskToIDs.insert({mask, id});
    return archetypes.back();
}

void ArchetypeManager::print() {
    std::cout << "archetypes:" << std::endl;
    for (const Archetype& a : archetypes) {
        std::cout << "  - id: "           << a.getID()          << std::endl;
        std::cout << "    mask: "         << a.getMask()        << std::endl;
        std::cout << "    capacity: "     << a.getCapacity()    << std::endl;
        std::cout << "    components: "                         << std::endl;
        std::cout << "    - id: "         << ENTITY_ID_NULL     << std::endl;
        std::cout << "      offset: "     << 0                  << std::endl;
        for (const Component& c : a.getComponents()) {
            std::cout << "    - id: "     << (int)c.getID()     << std::endl;
            std::cout << "      offset: " <<      c.getOffset() << std::endl;
        }
    }
}

} // namespace ECS