#pragma once

#include "ecs/types.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <vector>

namespace ECS {

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
    entities[id].nullify();
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
        std::cout << "  - id: "    << e.getID()       << std::endl;
        std::cout << "  - chunk: " << e.getChunkID()  << std::endl;
        std::cout << "  - index: " << e.getChunkIdx() << std::endl;
    }
}

} // namepsace ECS