#pragma once

#include "ecs/types.hpp"

#include <cstdint>
#include <limits> // for std::numeric_limits

namespace ECS {

using EntityID  = uint32_t;
using EntityIdx = uint16_t;

constexpr const EntityID  ENTITY_ID_NULL  = std::numeric_limits<EntityID >::max();
constexpr const EntityIdx ENTITY_IDX_NULL = std::numeric_limits<EntityIdx>::max();

class Entity {
public:
    // TODO: figure out proper interfaces
    friend class Archetype;
    friend class World;

    Entity();
    void invalidate();

private:
    EntityID id;     // unique entity identifier
    Chunk* chunk;    // entity exists in this chunk
    EntityIdx index; // entity exists in this chunk index
};

Entity::Entity() {
    invalidate();
}

void Entity::invalidate() {
    id = ENTITY_ID_NULL;
    chunk = nullptr;
    index = ENTITY_IDX_NULL;
}

} // namepsace ECS