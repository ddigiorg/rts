#pragma once

#include "ecs/types.hpp"

#include <array>

namespace ECS {

class Archetype {
public:
    // TODO: functions

private:
    static id_t count;

    id_t   id;
    mask_t mask;
    idx_t  chunkOpenIdx;
    idx_t  chunkEntityCapacity;
    std::array<size_t, COMPONENT_CAPACITY> idToOffset;
    std::array<id_t,   COMPONENT_CAPACITY> leftArchetypes;
    std::array<id_t,   COMPONENT_CAPACITY> rightArchetypes;

    std::vector<Chunk*> chunks;
};

} // namespace ECS