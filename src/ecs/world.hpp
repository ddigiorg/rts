#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <algorithm> // for std::sort
#include <array>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace ECS {

class World {
public:
    World();

    // entity functions
    // bool hasEntity(id_t entityId) const;
    id_t createEntity(const std::vector<id_t>& componentIds);
    // void removeEntity(id_t entityId);
    // template <typename C>
    // void insertEntityComponent();
    // template <typename C>
    // void removeEntityComponent();
    // template <typename C>
    // C* getEntityData(id_t entityId);
    // template <typename C>
    // void setEntityData(id_t entityId, const C& value);

    // component functions
    template <typename C>
    bool hasComponent() const;
    bool hasComponent(id_t id) const;
    template <typename C>
    bool isTag() const;
    bool isTag(id_t id) const;
    template <typename C>
    id_t registerComponent();
    template <typename C>
    id_t registerTag();

    // QueryIterator query(Query q);

    // print functions
    void print();
    void printArchetypes();
    // void printChunks();
    void printComponents();
    void printEntities();

private:

    // entity functions
    id_t _nextEntity();
    void _freeEntity(id_t entityId);

    // archetype functions
    id_t _getOrCreateArchetype(const std::vector<id_t>& componentIds);

    // entity data
    std::vector<Entity> entities{};
    std::vector<id_t> freeEntityIds{};
    id_t nextEntityId{0};

    // component data
    std::array<Component, Component::CAPACITY> components{};
    id_t componentCount{0};

    // archetype data
    std::unordered_map<mask_t, id_t> archetypeMaskToId{};
    std::vector<Archetype> archetypes{};
    id_t nextArchetypeId{0};

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
// Entity Functions
// =============================================================================

// bool World::hasEntity(id_t entityId) const {
//     // TODO: need to detect if id is in freeIds
//     return entityId < entities.size();
// }

id_t World::createEntity(const std::vector<id_t>& componentIds) {
    id_t eId = _nextEntity();
    id_t aId = _getOrCreateArchetype(componentIds); // TODO return reference instead
    Archetype& archetype = archetypes[aId];
    // archetype.insertEntity();
    return eId;
}

// void World::removeEntity(id_t entityId) {
//     ASSERT(entityId < eNextId, "EntityId " << entityId << " does not exist.");
//     // TODO: archetype and chunk stuff here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//     _freeEntity(entityId);
// }

// void World::insertEntityComponent(id_t entityId, id_t componentId) {
//     archetypeMgr.moveEntityRight(entityId, componentId)
// }

// void World::removeEntityComponent(id_t entityId, id_t componentId) {
//     archetypeMgr.moveEntityLeft(entityId, componentId)
// }

id_t World::_nextEntity() {
    id_t id = INVALID_ID;

    if (!freeEntityIds.empty()) {
        id = freeEntityIds.back();
        freeEntityIds.pop_back();
        entities[id].setId(id);
        return id;
    }

    id = nextEntityId++;
    entities.emplace_back();
    entities.back().setId(id);
    return id;
}

void World::_freeEntity(id_t id) {
    ASSERT(id < nextEntityId, "EntityId " << id << " does not exist.");
    entities[id].invalidate();
    freeEntityIds.push_back(id);
}

// =============================================================================
// Component Functions
// =============================================================================

template <typename C>
bool World::hasComponent() const {
    return hasComponent(getComponentId<C>());
}

bool World::hasComponent(id_t id) const {
    return components[id].getId() != INVALID_ID;
}

template <typename C>
bool World::isTag() const {
    return isTag(getComponentId<C>());
}

bool World::isTag(id_t id) const {
    return components[id].isTag();
}

template <typename C>
id_t World::registerComponent() {
    id_t id = getComponentId<C>();
    static_assert(std::is_base_of<IComponentData, C>::value, "Component must inherit from IComponentData base.");
    ASSERT(componentCount < Component::CAPACITY, "Component registry full.");
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already registered.");
    components[id].set(id, sizeof(C));
    componentCount++;
    return id;
}

template <typename C>
id_t World::registerTag() {
    id_t id = getComponentId<C>();
    static_assert(std::is_base_of<IComponentData, C>::value, "Component must inherit from IComponentData base.");
    ASSERT(componentCount < Component::CAPACITY, "Component registry full.");
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already registered.");
    components[id].set(id, 0);
    componentCount++;
    return id;
}

// =============================================================================
// Archetype Functions
// =============================================================================

id_t World::_getOrCreateArchetype(const std::vector<id_t>& componentIds) {

    // normalize component ids by sorting
    std::vector<id_t> sortedIds = componentIds;
    std::sort(sortedIds.begin(), sortedIds.end());

    // build archetype mask
    mask_t mask = 0;
    for (id_t cId : sortedIds) {
        mask |= (mask_t(1) << cId);
    }

    // if archetype already exists then return it
    auto it = archetypeMaskToId.find(mask);
    if (it != archetypeMaskToId.end()) {
        return it->second;
    }

    // otherwise allocate a new archetype
    id_t id = nextArchetypeId++;
    std::vector<Component*> componentPtrs; 
    for (id_t cId : sortedIds) {
        componentPtrs.push_back(&components[cId]);
    }
    archetypes.emplace_back(id, mask, componentPtrs);

    return id;
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
        std::cout << "  - id: "           << a.getId()            << std::endl;
        std::cout << "    mask: "         << a.getMask()          << std::endl;
        std::cout << "    chunkOpenIdx: " << a.getChunkOpenIdx()  << std::endl;
        std::cout << "    capacity: "     << a.getCapacity()      << std::endl;
        std::cout << "    chunkCount: "   << a.getChunkCount()    << std::endl;
        std::cout << "    components: "                           << std::endl;
        std::cout << "    - id: "         << INVALID_ID           << std::endl;
        std::cout << "      size: "       << sizeof(id_t)         << std::endl;
        std::cout << "      offset: "     << 0                    << std::endl;
        for (Component* c : a.getComponents()) {
            id_t id = c->getId();
            std::cout << "    - id: "     << id                   << std::endl;
            std::cout << "      size: "   << c->getSize()         << std::endl;
            std::cout << "      offset: " << a.getArrayOffset(id) << std::endl;
        }
    }
}

void World::printComponents() {
    std::cout << "components:" << std::endl;
    for (id_t id = 0; id < componentCount; id++) {
        const Component& c = components[id];
        std::cout << "  - id: "   << c.getId()   << std::endl;
        std::cout << "    size: " << c.getSize() << std::endl;
    }
}

void World::printEntities() {
    std::cout << "entities:" << std::endl;
    for (const Entity& e : entities) {
        std::cout << "  - id: "          << e.getId()        << std::endl;
        // std::cout << "  - archetypeId: " << e.archetypeId()  << std::endl;
        std::cout << "  - chunkIdx: "    << e.getChunkIdx()  << std::endl;
        std::cout << "  - entityIdx: "   << e.getEntityIdx() << std::endl;
    }
}

} // namespace ECS