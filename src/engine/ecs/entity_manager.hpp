#pragma once

#include "engine/ecs/types.hpp"
#include "engine/utilities/assert.hpp"

#include <vector>

namespace ECS {

class EntityManager {
public:
    ~EntityManager();

    bool hasEntity(const EntityID id);
    EntityID createEntity();
    void removeEntity(const EntityID id);
    void setEntity(const EntityID id, const EntityRecord& entity);
    const EntityRecord& getEntity(const EntityID id);

    const std::vector<EntityRecord*>& getEntities() const { return entities; };

private:
    std::vector<EntityRecord*> entities = {};
    std::vector<EntityID> recycledIDs = {};
};

EntityManager::~EntityManager() {
    for (EntityRecord*& entity : entities) {
        if (entity != nullptr)
            delete entity;
            entity = nullptr;
    }
}

bool EntityManager::hasEntity(const EntityID id) {
    if (id >= entities.size())
        return false;
    if (entities[id] == nullptr)
        return false;
    return true;
}

EntityID EntityManager::createEntity() {
    EntityID id = 0;
    if (recycledIDs.empty()) {
        id = getNextEntityID();
        entities.emplace_back(new EntityRecord{nullptr, 0, 0});
    }
    else {
        id = recycledIDs.back();
        recycledIDs.pop_back();
        entities[id] = new EntityRecord{nullptr, 0, 0};
    }
    return id;
}

void EntityManager::removeEntity(const EntityID id) {
    ASSERT(hasEntity(id), "Entity " << id << " does not exist.");
    delete entities[id];
    entities[id] = nullptr;
    recycledIDs.push_back(id);
}

void EntityManager::setEntity(const EntityID id, const EntityRecord& entity) {
    ASSERT(hasEntity(id), "Entity " << id << " does not exist.");
    *entities[id] = entity;
}

const EntityRecord& EntityManager::getEntity(const EntityID id) {
    ASSERT(hasEntity(id), "Entity " << id << " does not exist.");
    return *entities[id];
}

} // namespace ECS