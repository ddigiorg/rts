#pragma once

#include "ecs/types.hpp"
#include "utils/assert.hpp"

#include <vector>

namespace ECS {

// =============================================================================
// Entity
// =============================================================================

class Entity {
public:
    friend class EntityManager;

    Entity() { invalidate(); }
    void invalidate();

    EntityID getID() const { return id; }

private:
    EntityID id;     // unique entity identifier
    Chunk* chunk;    // entity exists in this chunk
    EntityIdx index; // entity exists in this chunk index
};

void Entity::invalidate() {
    id    = ENTITY_ID_NULL;
    chunk = nullptr;
    index = ENTITY_IDX_NULL;
}

// =============================================================================
// Entity Manager
// =============================================================================

class EntityManager {
public:
    EntityManager() : entities({}), freeIDs({}), nextID(0) {}

    Entity& allocateEntity();
    void freeEntity(EntityID id);

    // queries
    bool hasEntity(EntityID id) const;

    // getters
    Entity& getEntity(EntityID id);

    // miscellaneous
    void print();

private:
    std::vector<Entity> entities;
    std::vector<EntityID> freeIDs;
    EntityID nextID;
};

Entity& EntityManager::allocateEntity() {
    EntityID id = ENTITY_ID_NULL;

    if (!freeIDs.empty()) {
        id = freeIDs.back();
        freeIDs.pop_back();
        entities[id].id = id;
        return entities[id];
    }

    id = nextID++;
    entities.emplace_back();
    entities.back().id = id;
    return entities[id];
}

void EntityManager::freeEntity(EntityID id) {
    ASSERT(hasEntity(id), "EntityID " << id << " does not exist.");
    entities[id].invalidate();
    freeIDs.push_back(id);
}

bool EntityManager::hasEntity(EntityID id) const {
    if (id < entities.size())
        return !(entities[id].id == ENTITY_ID_NULL);
    return false;
}

Entity& EntityManager::getEntity(EntityID id) {
    ASSERT(hasEntity(id), "EntityID " << id << " does not exist.");
    return entities[id];
}

void EntityManager::print() {
    std::cout << "entities:" << std::endl;
    for (const Entity& e : entities) {
        if (e.id == ENTITY_ID_NULL) continue;
        std::cout << "  - id: "    << e.id    << std::endl;
        std::cout << "  - chunk: " << e.chunk << std::endl;
        std::cout << "  - index: " << e.index << std::endl;
    }
}

} // namepsace ECS