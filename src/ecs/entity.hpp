#pragma once

#include "ecs/types.hpp"

#include <cstdint>

namespace ECS {

class Entity {
public:
    Entity();
    void invalidate();
    void setId(id_t id_) { id = id_; }
    id_t getId() const { return id; }
    // id_t getArchetypeId() const { return archetype->getId(); }
    id_t getChunkIdx() const { return chunkIdx; }
    id_t getEntityIdx() const { return entityIdx; }

private:
    id_t id;              // unique entity identifier
    Archetype* archetype; // pointer to parent archetype
    uint32_t chunkIdx;    // this is the entity's chunk index in the archetype
    uint32_t entityIdx;   // this is the entity's index in the chunk
};

Entity::Entity() {
    invalidate();
}

void Entity::invalidate() {
    id = INVALID_ID;
    archetype = nullptr;
    chunkIdx = 0;
    entityIdx = 0;
}

} // namepsace ECS