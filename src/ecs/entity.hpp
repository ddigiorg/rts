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

    Entity() { reset(); }
    void reset();

    EntityID getID() const { return id; }

private:
    EntityID id;    // unique entity identifier
    Chunk*   chunk; // entity exists in this chunk
    ChunkIdx cIdx;  // entity exists in this chunk index
};

void Entity::reset() {
    id    = ENTITY_ID_NULL;
    chunk = nullptr;
    cIdx  = CHUNK_IDX_NULL;
}

// =============================================================================
// Entity Manager
// =============================================================================

class EntityManager {
public:
    EntityManager() : entities({}), freeIDs({}) {}

    Entity& newEntity();
    void freeEntity(EntityID id);
    bool hasEntity(EntityID id) const;
    Entity& getEntity(EntityID id);
    void print();

private:
    std::vector<Entity> entities;
    std::vector<EntityID> freeIDs;
};

Entity& EntityManager::newEntity() {
    EntityID id = ENTITY_ID_NULL;

    if (!freeIDs.empty()) {
        id = freeIDs.back();
        freeIDs.pop_back();
        entities[id].id = id;
        return entities[id];
    }

    id = static_cast<EntityID>(entities.size());
    entities.emplace_back();
    entities.back().id = id;
    return entities[id];
}

void EntityManager::freeEntity(EntityID id) {
    ASSERT(hasEntity(id), "EntityID " << id << " does not exist.");
    entities[id].reset();
    freeIDs.push_back(id);
}

bool EntityManager::hasEntity(EntityID id) const {
    if (id < static_cast<EntityID>(entities.size()))
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
        std::cout << "  - index: " << e.cIdx  << std::endl;
    }
}

} // namepsace ECS