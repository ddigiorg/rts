#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <algorithm> // for std::sort
#include <array>
#include <cstdint>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace ECS {

class World {
public:
    World();

    // component functions
    template <typename C>
    ComponentID registerComponent();
    template <typename C>
    ComponentID registerTag();
    template <typename C>
    bool hasComponent() const;
    bool hasComponent(ComponentID id) const;
    template <typename C>
    bool isTag() const;
    bool isTag(ComponentID id) const;
    template <typename C>
    const Component& getComponent() const;
    const Component& getComponent(ComponentID id) const;

    // entity functions
    EntityID createEntity(const std::vector<ComponentID>& cIDs);
    void removeEntity(EntityID id);
    // template <typename C>
    // void insertEntityComponent();
    // template <typename C>
    // void removeEntityComponent();
    bool hasEntity(EntityID id) const;
    // template <typename C>
    // C* getEntityData(EntityID id);
    // template <typename C>
    // void setEntityData(EntityID id, const C& value);



    // QueryIterator query(Query q);

    // print functions
    void print();
    void printArchetypes();
    // void printChunks();
    void printComponents();
    void printEntities();

private:

    // archetype functions
    ArchetypeID _getOrCreateArchetype(const std::vector<ComponentID>& cIDs);

    // entity functions
    EntityID _nextEntity();
    void _freeEntity(EntityID id);

    // archetype data
    std::unordered_map<mask_t, ArchetypeID> archetypeMaskToID;
    std::deque<Archetype> archetypes;
    ArchetypeID nextArchetypeID{0};

    // NOTE:
    // Using dequeue for stable archetypes because Chunk stores a raw pointer
    // back to its Archetype. If the container that holds Archetype objects
    // moves them, the pointer becomes invalid unless using deque or heap.

    // component data
    std::array<Component, COMPONENT_CAPACITY> components;
    ComponentID componentCount{0};

    // entity data
    std::vector<Entity> entities;
    std::vector<EntityID> freeEntityIDs;
    EntityID nextEntityID{0};

    // event data
    // query data
    // system data
};

// =============================================================================
// Constructor and Destructor
// =============================================================================

World::World() {
    _getOrCreateArchetype({}); // create default archetype with no components
}


// =============================================================================
// Archetype Functions
// =============================================================================

ArchetypeID World::_getOrCreateArchetype(const std::vector<ComponentID>& cIDs) {

    // normalize component ids by sorting
    std::vector<ComponentID> sortedIDs = cIDs;
    std::sort(sortedIDs.begin(), sortedIDs.end());

    // build archetype mask
    mask_t aMask = 0;
    for (ComponentID cID : sortedIDs) {
        aMask |= (mask_t(1) << cID);
    }

    // if archetype already exists then return it
    auto it = archetypeMaskToID.find(aMask);
    if (it != archetypeMaskToID.end()) {
        return it->second;
    }

    // otherwise allocate a new archetype
    ArchetypeID aID = nextArchetypeID++;
    std::vector<Component> archetypeComponents;
    for (ComponentID cID : sortedIDs) {
        archetypeComponents.push_back(components[cID]);
    }
    archetypeMaskToID.insert({aMask, aID});
    archetypes.emplace_back(aID, aMask, archetypeComponents);

    return aID;
}

// =============================================================================
// Component Functions
// =============================================================================

template <typename C>
ComponentID World::registerComponent() {
    ComponentID id = getComponentID<C>();
    ASSERT(componentCount < COMPONENT_CAPACITY, "Component registry full.");
    ASSERT(id < COMPONENT_CAPACITY, "Component id must be between 0 and 63");
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already registered.");
    static_assert(std::is_base_of<IComponentData, C>::value, "Component must inherit from IComponentData base.");

    components[id].id   = id,
    components[id].size = sizeof(C);
    components[id].mask = mask_t(1) << id;
    componentCount++;

    return id;
}

template <typename C>
ComponentID World::registerTag() {
    ComponentID id = getComponentID<C>();
    ASSERT(componentCount < COMPONENT_CAPACITY, "Component registry full.");
    ASSERT(id < COMPONENT_CAPACITY, "Component id must be between 0 and 63");
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already registered.");
    static_assert(std::is_base_of<IComponentData, C>::value, "Component must inherit from IComponentData base.");

    components[id].id   = id,
    components[id].size = 0;
    components[id].mask = mask_t(1) << id;
    componentCount++;

    return id;
}

template <typename C>
bool World::hasComponent() const {
    return hasComponent(getComponentID<C>());
}

bool World::hasComponent(ComponentID id) const {
    return components[id].id != COMPONENT_ID_NULL;
}

template <typename C>
bool World::isTag() const {
    return isTag(getComponentID<C>());
}

bool World::isTag(ComponentID id) const {
    return components[id].isTag();
}

template <typename C>
const Component& World::getComponent() const {
    return getComponent(getComponentID<C>());
}

const Component& World::getComponent(ComponentID id) const {
    ASSERT(hasComponent(id), "Component " << id << " does not exist.");
    return components[id];
}

// =============================================================================
// Entity Functions
// =============================================================================

EntityID World::createEntity(const std::vector<ComponentID>& cIDs) {
    EntityID    eID = _nextEntity();
    ArchetypeID aID = _getOrCreateArchetype(cIDs);
    archetypes[aID].insertEntity(eID, entities);
    ASSERT(entities[eID].chunk != nullptr, "entity.chunk is null after insert");
    ASSERT(entities[eID].chunk->archetype != nullptr, "chunk->archetype is null after insert");
    return eID;
}

void World::removeEntity(EntityID id) {
    ASSERT(hasEntity(id), "EntityID " << id << " does not exist.");
    Archetype* archetype = entities[id].chunk->archetype;
    archetype->removeEntity(id, entities);
    _freeEntity(id);
}

bool World::hasEntity(EntityID id) const {
    if (id < entities.size())
        return !(entities[id].id == ENTITY_ID_NULL);
    return false;
}

EntityID World::_nextEntity() {
    EntityID id = ENTITY_ID_NULL;

    if (!freeEntityIDs.empty()) {
        id = freeEntityIDs.back();
        freeEntityIDs.pop_back();
        entities[id].id = id;
        return id;
    }

    id = nextEntityID++;
    entities.emplace_back();
    entities.back().id = id;
    return id;
}

void World::_freeEntity(EntityID id) {
    ASSERT(id < nextEntityID, "EntityID " << id << " does not exist.");
    entities[id].invalidate();
    freeEntityIDs.push_back(id);
}

// =============================================================================
// Print Functions
// =============================================================================

void World::print() {
    printArchetypes();
    // printChunks();
    printComponents();
    printEntities();
}

void World::printArchetypes() {
    std::cout << "archetypes:" << std::endl;
    for (const Archetype& a : archetypes) {
        std::cout << "  - id: "           << a.id                   << std::endl;
        std::cout << "    mask: "         << a.mask                 << std::endl;
        std::cout << "    capacity: "     << a.capacity             << std::endl;
        std::cout << "    chunkCount: "   << a.chunks.size()        << std::endl;
        std::cout << "    components: "                             << std::endl;
        std::cout << "    - id: "         << ENTITY_ID_NULL         << std::endl;
        std::cout << "      size: "       << sizeof(EntityID)       << std::endl;
        std::cout << "      offset: "     << 0                      << std::endl;
        for (const Component& c : a.getComponents()) {
            std::cout << "    - id: "     << c.id                   << std::endl;
            std::cout << "      size: "   << c.size                 << std::endl;
            std::cout << "      offset: " << a.getArrayOffset(c.id) << std::endl;
        }
    }
}

void World::printComponents() {
    std::cout << "components:" << std::endl;
    for (ComponentID id = 0; id < componentCount; id++) {
        const Component& c = components[id];
        std::cout << "  - id: "   << (int)c.id << std::endl;
        std::cout << "    size: " << c.size    << std::endl;
    }
}

void World::printEntities() {
    std::cout << "entities:" << std::endl;
    for (const Entity& e : entities) {
        if (e.id == ENTITY_ID_NULL) continue;
        std::cout << "  - id: "    << e.id    << std::endl;
        std::cout << "  - chunk: " << e.chunk << std::endl;
        std::cout << "  - index: " << e.index << std::endl;
    }
}

} // namespace ECS