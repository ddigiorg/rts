#pragma once

#include <cstdint>
#include <limits> // for std::numeric_limits
#include <type_traits>

namespace ECS {

class Archetype;
class ArchetypeManager;
class Chunk;
class ChunkManager;
class Component;
class ComponentManager;
class Entity;
class EntityManager;

using mask_t = uint64_t; // 64 bit mask where each bit represents a component

// =============================================================================
// Archetype
// =============================================================================

using ArchetypeID   = uint16_t;
using ArchetypeMask = mask_t; // each bit represents a component

constexpr const ArchetypeID ARCHETYPE_ID_NULL = std::numeric_limits<ArchetypeID>::max();

// =============================================================================
// Chunk
// =============================================================================

static constexpr uint16_t CHUNK_TOTAL_SIZE  = 16 * 1024; // 16 kilobytes
static constexpr uint16_t CHUNK_HEADER_SIZE = 256;
static constexpr uint16_t CHUNK_BUFFER_SIZE = CHUNK_TOTAL_SIZE - CHUNK_HEADER_SIZE;
static constexpr size_t CHUNK_COMPONENT_CAPACITY = 16;

// struct ChunkListKey {
//     ArchetypeMask mask;
//     SharedComponentID sharedID;
// };

struct ChunkList {
	Chunk* headChunkActive;
	Chunk* tailChunkActive;
	Chunk* headChunkOpen;
	Chunk* tailChunkOpen;

	Archetype* archetype;
};

// =============================================================================
// Component Types
// =============================================================================

#define typeof(C) getComponentID<C>()

using ComponentID   = uint8_t;
using ComponentMask = mask_t; // only a single bit active
using ComponentSize = uint16_t;

using SharedComponentID = uint16_t;

constexpr const size_t COMPONENT_CAPACITY = sizeof(ComponentMask) * 8;

constexpr const ComponentID   COMPONENT_ID_NULL   = std::numeric_limits<ComponentID  >::max();
constexpr const ComponentMask COMPONENT_MASK_NULL = 0;
constexpr const ComponentSize COMPONENT_SIZE_NULL = std::numeric_limits<ComponentSize>::max();

template <typename T>
constexpr bool IsComponentType =
    std::is_trivially_constructible_v<T> &&
    std::is_trivially_destructible_v<T> &&
    std::is_trivially_copyable_v<T> &&
    std::is_standard_layout_v<T>;

template <typename T>
constexpr bool IsTagType = std::is_empty_v<T>;

inline ComponentID getNextComponentID() {
    static ComponentID counter = 0;
    return counter++;
}

template <typename C>
inline ComponentID getComponentID() {
    static ComponentID id = getNextComponentID();
    return id;
}

// =============================================================================
// Entity
// =============================================================================

using EntityID  = uint32_t;
using EntityIdx = uint16_t;

constexpr const EntityID  ENTITY_ID_NULL  = std::numeric_limits<EntityID >::max();
constexpr const EntityIdx ENTITY_IDX_NULL = std::numeric_limits<EntityIdx>::max();

} // namespace ECS