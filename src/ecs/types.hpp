#pragma once

#include <cstdint>

namespace ECS {

using mask_t = uint64_t; // 64 bit mask for set archetype components

class Archetype;
class Chunk;
class Component;
class Entity;

} // namespace ECS