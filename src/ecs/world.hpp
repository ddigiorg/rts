#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

namespace ECS {

class World {
public:
    World()
        : archetypeMgr(componentMgr) {}

    // component
    template <typename C>
    ComponentID registerComponent();
    template <typename C>
    bool hasComponent() const;
    bool hasComponent(ComponentID cID) const;
    template <typename C>
    bool isTag() const;
    bool isTag(ComponentID cID) const;

    // entity
    template<typename... Components>
    EntityID createEntity(Components&&... components);
    // void removeEntity(EntityID id);
    // template <typename C>
    // void insertEntityComponent();
    // template <typename C>
    // void removeEntityComponent();
    // bool hasEntity(EntityID id) const;
    // template <typename C>
    // C* getEntityData(EntityID id);
    // template <typename C>
    // void setEntityData(EntityID id, const C& value);

    // QueryIterator query(Query q);

    // miscellaneous
    void print();

private:
    ArchetypeManager archetypeMgr;
    // ChunkManager chunkMgr;
    ComponentManager componentMgr;
    EntityManager entityMgr;
    // EventManager eventMgr;
    // QueryManager queryMgr;
    // SystemManager systemMgr;
};

// =============================================================================
// Component Functions
// =============================================================================

template <typename C>
ComponentID World::registerComponent() {
    return componentMgr.registerComponent<C>().getID();
}

template <typename C>
bool World::hasComponent() const {
    return hasComponent(getComponentID<C>());
}

bool World::hasComponent(ComponentID cID) const {
    return componentMgr.hasComponent(cID);
}

template <typename C>
bool World::isTag() const {
    return isTag(getComponentID<C>());
}

bool World::isTag(ComponentID cID) const {
    return componentMgr.hasComponent(cID);
}

// =============================================================================
// Entity Functions
// =============================================================================

template<typename... Components>
EntityID World::createEntity(Components&&... components) {
    Entity& e = entityMgr.allocateEntity();
    Archetype& a = archetypeMgr.getOrCreateArchetype<Components...>();
    // chunkMgr.insertEntity(e, a, std::forward<Components>(components)...);
    return e.getID();
}

// =============================================================================
// Print Functions
// =============================================================================

void World::print() {
    archetypeMgr.print();
    // printChunks();
    componentMgr.print();
    entityMgr.print();
}

} // namespace ECS