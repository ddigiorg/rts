#pragma once

#include "ecs/types.hpp"

namespace ECS {

class Entity {
public:
    Entity() { nullify(); }
    void nullify();

    EntityID getID()       const { return id;       }
    ChunkID  getChunkID()  const { return chunkID;  };
    ChunkIdx getChunkIdx() const { return chunkIdx; };

private:
    friend class EntityManager;
    friend class ChunkManager;
    friend class World;

    void _setLocation(ChunkID chunkID, ChunkIdx chunkIdx);

    EntityID id;       // unique entity identifier
    ChunkID  chunkID;  // entity exists in this chunk
    ChunkIdx chunkIdx; // entity exists in this chunk index
};

void Entity::nullify() {
    id       = ENTITY_ID_NULL;
    chunkID  = CHUNK_ID_NULL;
    chunkIdx = CHUNK_IDX_NULL;
}

void Entity::_setLocation(ChunkID chunkID, ChunkIdx chunkIdx) {
    this->chunkID  = chunkID;
    this->chunkIdx = chunkIdx;
}

} // namepsace ECS