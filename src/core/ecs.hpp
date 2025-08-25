#pragma once

#include "utils/assert.hpp"

#include <algorithm> // for std::sort
#include <array>
#include <cstddef> // for std::byte
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
// Component
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
    id_t   componentId;
    size_t elementSize;
    size_t arrayOffset;
    size_t arraySize;
};

// =============================================================================
// Chunk
// =============================================================================

// A Chunk represents a contiguous block of memory for storing entity component 
// data.  It is designed for efficient memory access and L1 cache locality.

class Chunk {
public:
    Chunk(); // TODO

    // TODO
    uint32_t addEntity(); // returns new entity index
    void removeEntity(uint32_t index); // swap-pop for dense arrays
    void copyEntityTo(uint32_t index, Chunk& dst, uint32_t dstIndex);

    template <typename C>
    bool hasComponent() const;

    template <typename C>
    C* getArray();

    template <typename C>
    const C* getArray() const;

    template <typename C>
    C& getElement(uint32_t index);

    template <typename C>
    const C& getElement(uint32_t index) const;

    template <typename C>
    void setElement(uint32_t index, const C& value);

    bool isFull() const { return entityCount >= entityCapacity; }

    void incrementVersion() { ++version; }
    size_t getEntityCount() const { return entityCount; }
    size_t getCapacity() const { return entityCapacity; }
    Archetype* getArchetype() const { return archetype; }

    static constexpr size_t TOTAL_SIZE  = 16 * 1024;
    static constexpr size_t HEADER_SIZE = 128;
    static constexpr size_t BUFFER_SIZE = TOTAL_SIZE - HEADER_SIZE;

private:
    // --- Header (metadata, 256 bytes) ---
    uint32_t entityCount;       // num entities currently in this chunk
    uint32_t entityCapacity;    // max entities for this chunk
    uint32_t version;           // structural change version
    Archetype* archetype;       // pointer to archetype definition
    void* sharedComponentArray; // pointer to shared component data

    // --- Data buffer (aligned packed storage, 16 KB - 256 B) ---
    alignas(64) std::array<std::byte, BUFFER_SIZE> buffer;
};

template <typename C>
bool Chunk::hasComponent() const {
    return archetype->hasComponent(getComponentId<C>());
}

template <typename C>
C* Chunk::getArray() {
    ASSERT(archetype != nullptr, "Chunk has no archetype set.");
    size_t offset = archetype->getOffset(getComponentId<C>());
    ASSERT(offset + sizeof(C) * entityCount <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return reinterpret_cast<C*>(buffer.data() + offset);
}

template <typename C>
const C* Chunk::getArray() const {
    ASSERT(archetype != nullptr, "Chunk has no archetype set.");
    size_t offset = archetype->getOffset(getComponentId<C>());
    ASSERT(offset + sizeof(C) * entityCount <= BUFFER_SIZE,
           "Component array exceeds chunk buffer.");
    return reinterpret_cast<const C*>(buffer.data() + offset);
}

template <typename C>
C& Chunk::getElement(uint32_t index) {
    ASSERT(index < entityCount, "Index out of bounds.");
    return getArray<C>()[index];
}

template <typename C>
const C& Chunk::getElement(uint32_t index) const {
    ASSERT(index < entityCount, "Index out of bounds.");
    return getArray<C>()[index];
}

template <typename C>
void setElement(uint32_t index, const C& value) {
    ASSERT(index < entityCount, "Index out of bounds.");
    getArray<C>()[index] = value;
}

// =============================================================================
// Archetype
// =============================================================================

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

// =============================================================================
// Manager Class
// =============================================================================

class Manager {
public:
    Manager();
    ~Manager();

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
    void _pushChunkOnArchetype(id_t archetypeId);
    void _popChunkOffArchetype(id_t archetypeId);
    void _freeChunks();

    // entity data
    std::vector<EntityRecord> entities{};
    std::vector<id_t> eFreeIds{};
    id_t eNextId{0};

    // component data
    std::array<ComponentRecord, COMPONENT_CAPACITY> components{};
    id_t cCount{0};

    // archetype data
    std::vector<Archetype> archetypes;
    std::unordered_map<mask_t, id_t> archetypeMaskToId;

    // EventManager eventMgr;
    // QueryManager queryMgr;
    // SystemManager systemMgr;
};

// =============================================================================
// Constructor and Destructor
// =============================================================================

Manager::Manager() {

    for (auto& c : components) {
        c.componentId = INVALID_ID;
        c.elementSize = 0;
        c.arrayOffset = 0;
        c.arraySize   = 0;
    }

    // create default archetype with no components
    _getOrCreateArchetype({});
}

Manager::~Manager() {
    _freeChunks();
}

// =============================================================================
// Entity Functions
// =============================================================================

bool Manager::hasEntity(id_t entityId) const {
    // TODO: need to detect if id is in freeIds
    return entityId < entities.size();
}

id_t Manager::createEntity(const std::vector<id_t>& componentIds) {
    id_t eId = _nextEntity();
    id_t aId = _getOrCreateArchetype(componentIds);
    std::vector<ChunkMeta >& chunkMeta = aChunkMeta[aId];
    std::vector<ChunkData*>& chunkData = aChunkData[aId];

    // allocate new chunk if no chunks exist in archetype
    if (chunkData.empty()) {
        _pushChunkOnArchetype(aId);
    }

    // TODO: archetype and chunk stuff here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    return eId;
}

void Manager::removeEntity(id_t entityId) {
    ASSERT(entityId < eNextId, "EntityId " << entityId << " does not exist.");
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

    if (!eFreeIds.empty()) {
        entityId = eFreeIds.back();
        eFreeIds.pop_back();
        entities[entityId].entityId = entityId;
        return entityId;
    }

    entityId = eNextId++;
    // TODO: is this optimal?
    EntityRecord entity;
    entity.clear();
    entity.entityId = entityId;
    entities.push_back(entity);
    return entityId;
}

void Manager::_freeEntity(id_t entityId) {
    ASSERT(entityId < eNextId, "EntityId " << entityId << " does not exist.");
    // TODO: need to check duplicate ids
    entities[entityId].clear();
    eFreeIds.push_back(entityId);
}

// =============================================================================
// Component Functions
// =============================================================================

template <typename C>
bool Manager::hasComponent() const {
    return hasComponent(getComponentId<C>());
}

bool Manager::hasComponent(id_t componentId) const {
    return componentId < cCount;
}

template <typename C>
bool Manager::isTag() const {
    return isTag(getComponentId<C>());
}

bool Manager::isTag(id_t componentId) const {
    return components[componentId].elementSize == 0;
}

template <typename C>
id_t Manager::registerComponent() {
    static_assert(std::is_base_of<Component, C>::value, "Component must inherit from Component base.");
    ASSERT(cCount < COMPONENT_CAPACITY, "Component registry full.");

    id_t id = getComponentId<C>();
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already exists.");

    components[id] = {
        /*componentId=*/ id,
        /*elementSize=*/ sizeof(C),
        /*arrayOffset=*/ 0,
        /*  arraySize=*/ 0,
    };

    cCount++;
    return id;
}

template <typename C>
id_t Manager::registerTag() {
    static_assert(std::is_base_of<Tag, C>::value, "Tag must inherit from Tag base.");
    ASSERT(cCount < COMPONENT_CAPACITY, "Component registry full.");

    id_t id = getComponentId<C>();
    ASSERT(!hasComponent(id), "Tag \"" << typeid(C).name() << "\" already exists.");

    components[id] = {
        /*componentId=*/ id,
        /*elementSize=*/ 0,
        /*arrayOffset=*/ 0,
        /*  arraySize=*/ 0,
    };

    cCount++;
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
    mask_t aMask = 0;
    for (id_t cId : sortedIds) {
        aMask |= (mask_t(1) << cId);
    }

    // check if archetype already exists and return it
    auto it = aMaskToId.find(aMask);
    if (it != aMaskToId.end()) {
        return it->second;
    }

    // allocate new archetype
    id_t aId = aNextId++;
    ArchetypeRecord archetype {
        /*   archetypeId=*/ aId,
        /* archetypeMask=*/ aMask,
        /*  openChunkIdx=*/ 0,
        /*entityCapacity=*/ 0,
        /*    components=*/ {},
        /*     neighbors=*/ {},
    };
    archetype.components.reserve(sortedIds.size() + 1);

    // add entity id component
    size_t aSize = 0;
    size_t idSize = sizeof(id_t);
    archetype.components.push_back({ INVALID_ID, idSize, 0});
    aSize += idSize;

    // gather component metadata
    for (id_t cId : sortedIds) {
        const ComponentRecord& c = components[cId];
        archetype.components.push_back(
            ComponentRecord {
                /*componentId=*/ c.componentId,
                /*elementSize=*/ c.elementSize,
                /*arrayOffset=*/ 0,
                /*  arraySize=*/ 0, 
            }
        );
        aSize += c.elementSize;
    }

    ASSERT(aSize < CHUNK_DATA_SIZE, "Archetype size exceeds chunk capacity");

    archetype.entityCapacity = (aSize > 0) ? CHUNK_DATA_SIZE / aSize : CHUNK_DATA_SIZE;

    // assign component offsets
    size_t offset = 0;
    size_t size = 0;
    for (auto& ac : archetype.components) {
        ac.arrayOffset = offset;
        ac.arraySize = ac.elementSize * archetype.entityCapacity;
        offset += ac.arraySize;
    }

    // append new archetype
    aMaskToId[aMask] = aId;
    archetypes.push_back(std::move(archetype));
    aChunkMeta.push_back({});
    aChunkData.push_back({});

    return aId;
}

void Manager::_pushChunkOnArchetype(id_t archetypeId) {
    ASSERT(archetypeId < archetypes.size(), "Archetype " << archetypeId << " does not exist.");
    ArchetypeRecord& aRecord = archetypes[archetypeId];

    ChunkMeta cMeta {
        /*   archetypeId=*/ archetypeId,
        /*      chunkIdx=*/ (idx_t)aChunkData[archetypeId].size(),
        /*   entityCount=*/ 0,
        /*entityCapacity=*/ aRecord.entityCapacity,
        /*    cidToArray=*/ {},
    };

    ChunkData* cData = static_cast<ChunkData*>(malloc(CHUNK_DATA_SIZE));

    for (const auto& c : aRecord.components) {
        if (c.componentId == INVALID_ID) continue;
        ChunkArrayRecord& ca = cMeta.cidToArray[c.componentId];
        ca.addr = reinterpret_cast<void*>(reinterpret_cast<std::byte*>(cData) + c.arrayOffset);
        ca.size = c.arraySize;
    }

    aChunkMeta[archetypeId].push_back(cMeta);
    aChunkData[archetypeId].push_back(cData);
}

void Manager::_popChunkOffArchetype(id_t archetypeId) {

}

void Manager::_freeChunks() {
    for (auto& chunks : aChunkData) {
        for (auto chunk : chunks) {
            free(chunk);
        }
    }
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
    for (const auto& e : entities) {
        std::cout << "  - entityId: "    << e.entityId    << std::endl;
        std::cout << "  - archetypeId: " << e.archetypeId << std::endl;
        std::cout << "  - chunkIdx: "    << e.chunkIdx    << std::endl;
        std::cout << "  - entityIdx: "   << e.entityIdx   << std::endl;
    }
}

void Manager::printComponents() {
    std::cout << "components:" << std::endl;
    for (id_t id = 0; id < cCount; id++) {
        const ComponentRecord& c = components[id];
        std::cout << "  - componentId: " << c.componentId << std::endl;
        std::cout << "    elementSize: " << c.elementSize << std::endl;
    }
}

void Manager::printArchetypes() {
    std::cout << "archetypes:" << std::endl;
    for (const auto& a : archetypes) {
        std::cout << "  - archetypeId: "    << a.archetypeId    << std::endl;
        std::cout << "    archetypeMask: "  << a.archetypeMask  << std::endl;
        std::cout << "    openChunkIdx: "   << a.openChunkIdx   << std::endl;
        std::cout << "    entityCapacity: " << a.entityCapacity << std::endl;
        std::cout << "    components: "                         << std::endl;
        for (const auto& c : a.components) {
            std::cout << "    - componentId: " << c.componentId << std::endl;
            std::cout << "      elementSize: " << c.elementSize << std::endl;
            std::cout << "      arrayOffset: " << c.arrayOffset << std::endl;
            std::cout << "      arraySize: "   << c.arraySize   << std::endl;
        }
    }
}

} // namespace ECS