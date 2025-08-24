#pragma once

#include "utils/assert.hpp"

#include <algorithm> // for std::sort
#include <array>
#include <cstdint>
#include <iostream>
#include <limits> // for std::numeric_limits
#include <unordered_map>
#include <vector>

namespace ECS {

// =============================================================================
// Common Types
// =============================================================================

using id_t   = uint32_t;
using idx_t  = uint32_t;
using mask_t = uint64_t;

constexpr const  id_t INVALID_ID  = std::numeric_limits< id_t>::max();
constexpr const idx_t INVALID_IDX = std::numeric_limits<idx_t>::max();

// =============================================================================
// Entity Types
// =============================================================================

struct EntityRecord {
    id_t  entityId;
    id_t  archetypeId;
    idx_t chunkIdx;
    idx_t entityIdx;

    void clear() {
        entityId    = INVALID_ID;
        archetypeId = INVALID_ID;
        chunkIdx    = INVALID_IDX;
        entityIdx   = INVALID_IDX;
    }
};

// =============================================================================
// Component Types
// =============================================================================

#define cidof(C) getComponentId<C>()

constexpr const size_t COMPONENT_CAPACITY = sizeof(mask_t) * 8;

struct Tag {};
struct Component {};

inline id_t getNextComponentId() {
    static id_t counter = 0;
    return counter++;
}

template <typename C>
inline id_t getComponentId() {
    static id_t id = getNextComponentId();
    return id;
}

struct ComponentRecord {
    id_t   id;
    size_t size;
    size_t offset;

    void clear() {
        id     = INVALID_ID;
        size   = 0;
        offset = 0;
    }
};

// =============================================================================
// Chunk Types
// =============================================================================

// A Chunk represents a contiguous block of memory for storing entity component 
// data.  It is designed for efficient memory access and L1 cache locality.
using ChunkData = char;

// TODO: I guess I dont need the header anymore since Im using ChunkRecord to store chunk metadata
// 16 kilobytes minus 256 byte header
// constexpr const size_t CHUNK_DATA_SIZE = 16 * 1024 - 256;
constexpr const size_t CHUNK_DATA_SIZE = 16 * 1024;

struct ChunkArrayRecord {
    void*  addr;
    size_t size;
};

struct ChunkRecord {
    idx_t  chunkIdx;
    id_t   archetypeId;
    size_t entityCount;
    size_t entityCapacity;
    std::array<ChunkArrayRecord, COMPONENT_CAPACITY> cidToArray;
};

// =============================================================================
// Archetype Types
// =============================================================================

struct ArchetypeNeighbors {
    id_t componentId;
    id_t left;
    id_t right;
};

struct ArchetypeRecord {
    id_t   id;
    mask_t mask;
    size_t entityCapacity;
    std::vector<ComponentRecord> components;
    std::array<ArchetypeNeighbors, COMPONENT_CAPACITY> neighbors;
};

// =============================================================================
// Manager
// =============================================================================

class Manager {
public:
    Manager();

    // entity functions
    bool hasEntity(id_t entityId) const;
    id_t createEntity(const std::vector<id_t>& componentIds);
    void removeEntity(id_t entityId);
    // template <typename C>
    // void insertEntityComponent();
    // template <typename C>
    // void removeEntityComponent();
    // template <typename C>
    // C* getEntityData(id_t entityId);
    // template <typename C>
    // void setEntityData(id_t entityId, const C& value);

    // component functions
    template <typename C>
    bool hasComponent() const;
    bool hasComponent(id_t componentId) const;
    template <typename C>
    bool isTag() const;
    bool isTag(id_t componentId) const;
    template <typename C>
    id_t registerComponent();
    template <typename C>
    id_t registerTag();

    // QueryIterator query(Query q);

    // print functions
    void print();
    void printEntities();
    void printComponents();
    void printArchetypes();

private:

    // entity functions
    id_t _nextEntity();
    void _freeEntity(id_t entityId);

    // archetype functions
    id_t _getOrCreateArchetype(const std::vector<id_t>& componentIds);

    // entity data
    std::vector<EntityRecord> entities{};
    std::vector<id_t> freeEntityIds{};
    id_t nextEntityId{0};

    // component data
    std::array<ComponentRecord, COMPONENT_CAPACITY> components{};
    id_t componentCount{0};

    // archetype data
    std::unordered_map<mask_t, id_t> maskToId;
    std::vector<ArchetypeRecord> archetypes;
    std::vector<std::vector<ChunkRecord>> chunkMeta;
    std::vector<std::vector<ChunkData*>>  chunkData;
    id_t nextId{0};

    // EventManager eventMgr;
    // QueryManager queryMgr;
    // SystemManager systemMgr;
};

// =============================================================================
// Constructor
// =============================================================================

Manager::Manager() {
    // TODO: need this?
    for (auto& component : components) {
        component.clear();
    }

    // create default archetype with no components
    _getOrCreateArchetype({});
}

// =============================================================================
// Entity Functions
// =============================================================================

bool Manager::hasEntity(id_t entityId) const {
    // TODO: need to detect if id is in freeIds
    return entityId < entities.size();
}

id_t Manager::createEntity(const std::vector<id_t>& componentIds) {
    id_t entityId = _nextEntity();
    id_t archetypeId = _getOrCreateArchetype(componentIds);
    // TODO: archetype and chunk stuff here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    return entityId;
}

void Manager::removeEntity(id_t entityId) {
    ASSERT(entityId < nextEntityId, "EntityId does not exist.");
    // TODO: archetype and chunk stuff here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    _freeEntity(entityId);
}

// void Manager::insertEntityComponent(id_t entityId, id_t componentId) {
//     archetypeMgr.moveEntityRight(entityId, componentId)
// }

// void Manager::removeEntityComponent(id_t entityId, id_t componentId) {
//     archetypeMgr.moveEntityLeft(entityId, componentId)
// }


id_t Manager::_nextEntity() {
    id_t entityId = INVALID_ID;

    if (!freeEntityIds.empty()) {
        entityId = freeEntityIds.back();
        freeEntityIds.pop_back();
        entities[entityId].entityId = entityId;
        return entityId;
    }

    entityId = nextEntityId++;
    // TODO: is this optimal?
    EntityRecord entity;
    entity.clear();
    entity.entityId = entityId;
    entities.push_back(entity);
    return entityId;
}

void Manager::_freeEntity(id_t entityId) {
    ASSERT(entityId < nextEntityId, "EntityId does not exist.");
    // TODO: need to check duplicate ids
    entities[entityId].clear();
    freeEntityIds.push_back(entityId);
}

// =============================================================================
// Component Functions
// =============================================================================

template <typename C>
bool Manager::hasComponent() const {
    return hasComponent(getComponentId<C>());
}

bool Manager::hasComponent(id_t componentId) const {
    return componentId < componentCount;
}

template <typename C>
bool Manager::isTag() const {
    return isTag(getComponentId<C>());
}

bool Manager::isTag(id_t componentId) const {
    return components[componentId].size == 0;
}

template <typename C>
id_t Manager::registerComponent() {
    static_assert(std::is_base_of<Component, C>::value, "Component must inherit from Component base.");
    ASSERT(componentCount < COMPONENT_CAPACITY, "Component registry full.");
    id_t id = getComponentId<C>();
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already exists.");
    components[id] = { id, sizeof(C), 0 };
    componentCount++;
    return id;
}

template <typename C>
id_t Manager::registerTag() {
    static_assert(std::is_base_of<Tag, C>::value, "Tag must inherit from Tag base.");
    ASSERT(componentCount < COMPONENT_CAPACITY, "Component registry full.");
    id_t id = getComponentId<C>();
    ASSERT(!hasComponent(id), "Tag \"" << typeid(C).name() << "\" already exists.");
    components[id] = { id, 0, 0 };
    componentCount++;
    return id;
}

// =============================================================================
// Archetype Functions
// =============================================================================

id_t Manager::_getOrCreateArchetype(const std::vector<id_t>& componentIds) {

    // sort input component ids to normalize
    std::vector<id_t> sortedIds = componentIds;
    std::sort(sortedIds.begin(), sortedIds.end());

    // build mask
    mask_t mask = 0;
    for (id_t cId : sortedIds) {
        mask |= (mask_t(1) << cId);
    }

    // check if archetype already exists and return it
    auto it = maskToId.find(mask);
    if (it != maskToId.end()) {
        return it->second;
    }

    // allocate new archetype
    size_t size = 0;
    id_t id = nextId++;
    ArchetypeRecord archetype{ id, mask, 0, {}, {} };
    archetype.components.reserve(sortedIds.size() + 1);

    // add entity id component
    size_t idSize = sizeof(id_t);
    archetype.components.push_back({ INVALID_ID, idSize, 0});
    size += idSize;

    // gather component metadata
    for (id_t cId : sortedIds) {
        const ComponentRecord& component = components[cId];
        archetype.components.push_back({ component.id, component.size, 0 });
        size += component.size;
    }

    ASSERT(size < CHUNK_DATA_SIZE, "Archetype size exceeds chunk capacity");

    archetype.entityCapacity = (size > 0) ? CHUNK_DATA_SIZE / size : CHUNK_DATA_SIZE;

    // assign component offsets
    size_t offset = 0;
    for (auto& archetypeComponent : archetype.components) {
        archetypeComponent.offset = offset;
        offset += archetype.entityCapacity * archetypeComponent.size;
    }

    archetypes.push_back(std::move(archetype));
    maskToId[mask] = id;
    return id;
}

// =============================================================================
// Print Functions
// =============================================================================

void Manager::print() {
    printEntities();
    printComponents();
    printArchetypes();
}

void Manager::printEntities() {
    std::cout << "entities:" << std::endl;
    for (const auto& entity : entities) {
        std::cout << "  - entityId: "    << entity.entityId    << std::endl;
        std::cout << "  - archetypeId: " << entity.archetypeId << std::endl;
        std::cout << "  - chunkIdx: "    << entity.chunkIdx    << std::endl;
        std::cout << "  - entityIdx: "   << entity.entityIdx   << std::endl;
    }
}

void Manager::printComponents() {
    std::cout << "components:" << std::endl;
    for (id_t id = 0; id < componentCount; id++) {
        const ComponentRecord& component = components[id];
        std::cout << "  - id: "   << component.id   << std::endl;
        std::cout << "    size: " << component.size << std::endl;
    }
}

void Manager::printArchetypes() {
    std::cout << "archetypes:" << std::endl;
    for (const auto& archetype : archetypes) {
        std::cout << "  - id: "             << archetype.id             << std::endl;
        std::cout << "    mask: "           << archetype.mask           << std::endl;
        std::cout << "    entityCapacity: " << archetype.entityCapacity << std::endl;
        std::cout << "    components: "                                 << std::endl;
        for (const auto& component : archetype.components) {
            std::cout << "    - id: "     << component.id     << std::endl;
            std::cout << "      size: "   << component.size   << std::endl;
            std::cout << "      offset: " << component.offset << std::endl;
        }
    }
}

} // namespace ECS