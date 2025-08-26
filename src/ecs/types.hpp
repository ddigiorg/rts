#pragma once

#include <cstdint>
#include <limits> // for std::numeric_limits

namespace ECS {

using id_t   = uint32_t;
using idx_t  = uint32_t;
using mask_t = uint64_t;

constexpr const  id_t INVALID_ID  = std::numeric_limits< id_t>::max();
constexpr const idx_t INVALID_IDX = std::numeric_limits<idx_t>::max();

class Archetype;
class Chunk;
class Component;
class Entity;

} // namespace ECS