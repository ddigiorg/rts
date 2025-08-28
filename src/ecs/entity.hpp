#pragma once

#include "ecs/types.hpp"

#include <cstdint>

namespace ECS {

class Entity {
public:
    // TODO: figure out proper interfaces
    friend class Archetype;
    friend class World;

    Entity();
    void invalidate();

private:
    id_t id;        // unique entity identifier
    Chunk* chunk;   // entity exists in this chunk
    uint32_t index; // entity exists in this chunk index
};

Entity::Entity() {
    invalidate();
}

void Entity::invalidate() {
    id = INVALID_ID;
    chunk = nullptr;
    index = 0;
}

} // namepsace ECS