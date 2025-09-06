#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/archetype_manager.hpp"
#include "ecs/chunk.hpp"
#include "ecs/chunk_manager.hpp"
#include "ecs/component.hpp"
#include "ecs/component_manager.hpp"
#include "ecs/entity.hpp"
#include "ecs/entity_manager.hpp"
#include "utils/assert.hpp"

namespace ECS {

// =============================================================================
// World
// =============================================================================

class World {
public:
    World();

    // chunk functions
    Chunk& getChunk(ChunkID id);

    // component functions
    template <typename C>
    ComponentID registerComponent();
    template <typename C>
    bool hasComponent() const;
    bool hasComponent(ComponentID cID) const;
    template <typename C>
    bool isTag() const;
    bool isTag(ComponentID cID) const;

    // entity functions
    template<typename... Components>
    EntityID createEntity(Components&&... data);
    template<typename... Components>
    EntityID createEntityInGroup(GroupID gID, Components&&... data);

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

    // miscellaneous functions
    void print();

private:
    ArchetypeManager archetypeMgr;
    ChunkManager chunkMgr;
    ComponentManager componentMgr;
    EntityManager entityMgr;
    // EventManager eventMgr;
    // QueryManager queryMgr;
    // SystemManager systemMgr;
};

// =============================================================================
// World Constructor
// =============================================================================

World::World()
    : archetypeMgr(componentMgr),
      chunkMgr(archetypeMgr, entityMgr),
      componentMgr({}),
      entityMgr({}) {}

// =============================================================================
// World Chunk Functions
// =============================================================================

Chunk& World::getChunk(ChunkID id) {
    return chunkMgr.getChunk(id);
}

// =============================================================================
// World Component Functions
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
// World Entity Functions
// =============================================================================

template<typename... Components>
EntityID World::createEntity(Components&&... data) {
    return createEntityInGroup(0, std::forward<Components>(data)...);
}

template<typename... Components>
EntityID World::createEntityInGroup(GroupID gID, Components&&... data) {
    // TODO: validate componentes are registered
    // (void)std::initializer_list<int>{(registerComponent<Components>(), 0)...};
    EntityID    eID = entityMgr.createEntity();
    ArchetypeID aID = archetypeMgr.getOrCreateArchetype<Components...>();
    chunkMgr.insertEntity(eID, aID, gID, std::forward<Components>(data)...);
    return eID;
}

// =============================================================================
// World Miscellaneous Functions
// =============================================================================

void World::print() {
    archetypeMgr.print();
    chunkMgr.print();
    componentMgr.print();
    entityMgr.print();
}

} // namespace ECS