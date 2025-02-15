#pragma once

#include "ecs/types.hpp"
#include "ecs/entity_manager.hpp"
#include "utils/assert.hpp"

#include <vector>
#include <unordered_map>
#include <cstring> // for std::memcpy
#include <algorithm> // for std::find

namespace ECS {

using ComponentMap = std::unordered_map<ComponentID, ComponentRecord>;

// A Chunk represents a contiguous block of memory for storing entity component 
// data.  It is designed for efficient memory access and L1 cache locality.
struct Chunk {
    using byte = char;
    static const size_t SIZE = 16 * 1024 - 256; // 16 kilobytes minus header overhead
    size_t index;    // chunk number in the archetype
    size_t count;    // current number of entities in the chunk
    size_t capacity; // maximum number of entities in the chunk
    Archetype* arch; // pointer to the parent Archetype
    byte buffer;     // buffer start
};

struct ArchetypeGraphNode {
    Archetype* left;
    Archetype* right;
};

inline void copyEntityComponetData(
    const EntityRecord& src,
    const EntityRecord& dst,
    bool useDstSig = false
);

class Archetype {
public:
    Archetype(
        const Hash hash,
        const Signature& sig,
        EntityManager* entityManager,
        const std::vector<ComponentRecord>& registeredComponents
    );

    ~Archetype();

    // checker functions
    bool hasEntity(const EntityID id);
    bool hasComponent(const ComponentID id);
    bool hasComponents(const Signature& sig);

    // entity functions
    void insertEntity(const EntityID id);
    void removeEntity(const EntityID id);
    void moveEntityRight(const EntityID eid, const ComponentID cid);
    void moveEntityLeft(const EntityID eid, const ComponentID cid);

    // graph functions
    Archetype* getRightNode(const ComponentID id);
    Archetype* getLeftNode(const ComponentID id);
    void setRightNode(Archetype* arch, const ComponentID id);
    void setLeftNode(Archetype* arch, const ComponentID id);

    // chunk component data functions
    void* getBufferLocation(const ComponentID cid, const size_t col, const size_t row);
    template <typename T>
    T* getComponentDataArray(size_t col);
    template <typename T>
    T* getComponentData(size_t col, size_t row);
    template <typename T>
    void clearComponentData(size_t col, size_t row);
    void clearComponentData(ComponentID id, size_t col, size_t row);

    // getters
    Hash getHash() const { return hash; };
    const Signature& getSignature() const { return signature; };
    size_t getNumEntities() const { return entities.size(); };
    size_t getNumChunks() const { return chunks.size(); };
    size_t getChunkNumEntities(size_t c) const { return chunks[c]->count; };
    size_t getChunkMaxEntities(size_t c) const { return chunks[c]->capacity; };
    const ComponentMap& getComponents() const { return components; };

private:
    void _allocateBackChunk();
    void _deallocateBackChunk();

    Hash hash;
    Signature signature;
    size_t maxEntitiesPerChunk;
    std::vector<EntityID> entities;
    std::vector<Chunk*> chunks;
    ComponentMap components;
    std::unordered_map<ComponentID, ArchetypeGraphNode> nodes;
    EntityManager* entityManager;
};

// =============================================================================
// Constructor and Destructor Functions
// =============================================================================

Archetype::Archetype(
        const Hash hash,
        const Signature& sig,
        EntityManager* entityManager,
        const std::vector<ComponentRecord>& registeredComponents
) {
    size_t entitySize = 0;
    size_t offset = 0;

    this->hash = hash;
    this->signature = sig;
    this->entityManager = entityManager;

    // calculate size of entity data
    for (const ComponentID& id : signature)
        entitySize += registeredComponents[id].size;
    ASSERT(entitySize < Chunk::SIZE, "Component data size larger than chunk buffer size!");

    // calculate numEntities based on chunk buffer size and size of entity data
    this->maxEntitiesPerChunk = Chunk::SIZE;
    if (entitySize > 0)
        this->maxEntitiesPerChunk = (size_t)(Chunk::SIZE / entitySize);

    // set components
    for (const ComponentID& id : signature) {
        size_t size = registeredComponents[id].size;
        components.emplace(id, ComponentRecord{size, offset});
        offset += maxEntitiesPerChunk * size;
    }
}

Archetype::~Archetype() {
    for (Chunk* chunk : chunks) {
        if (chunk != nullptr)
            free(chunk);
    }
}

// =============================================================================
// Checker Functions
// =============================================================================

bool Archetype::hasEntity(const EntityID id) {
    if (entityManager->hasEntity(id) == false)
        return false;
    EntityRecord entity = entityManager->getEntity(id);
    if (entity.arch != this)
        return false;
    return true;
}

bool Archetype::hasComponent(const ComponentID id) {
    if (components.find(id) != components.end())
        return true;
    return false;
}

bool Archetype::hasComponents(const Signature& sig) {
    for (const ComponentID& id : sig) {
        if (!hasComponent(id))
            return false;
    }
    return true;
}

// =============================================================================
// Entity Functions
// =============================================================================

void Archetype::insertEntity(const EntityID id) {
    ASSERT(!hasEntity(id), "Entity " << id << "is already in Archetype " << hash << ".");

    // allocate new chunk if no chunks exist
    if (chunks.empty())
        _allocateBackChunk();

    // allocate new chunk if back chunk is full
    Chunk* dstChunk = chunks.back();
    if (dstChunk->count == dstChunk->capacity) {
        _allocateBackChunk();
        dstChunk = chunks.back();
    }

    // insert entity to chunk
    entities.push_back(id);
    dstChunk->count++;

    // clear entity's component data
    EntityRecord entity = EntityRecord{this, chunks.size()-1, dstChunk->count-1};
    for (const ComponentID id : signature)
        clearComponentData(id, entity.col, entity.row);

    // update entity manager
    entityManager->setEntity(id, entity);
}

void Archetype::removeEntity(const EntityID id) {
    ASSERT(hasEntity(id), "Entity " << id << " does not exist in Archetype " << hash << ".");

    Chunk* srcChunk = chunks.back();
    EntityID srcID = entities.back();
    EntityID dstID = id;
    EntityRecord srcEntity = EntityRecord{this, srcChunk->index, srcChunk->count-1};
    EntityRecord dstEntity = entityManager->getEntity(id);

    // move entity component data from back to fill the missing spot
    copyEntityComponetData(srcEntity, dstEntity);

    // move entity id from back index to the removed index
    size_t i = dstEntity.col * maxEntitiesPerChunk + dstEntity.row;
    entities[i] = srcID;
    entities.pop_back();
    srcChunk->count--;

    // deallocate back chunk if it is empty
    if (srcChunk->count == 0)
        _deallocateBackChunk();

    // update entity manager
    entityManager->setEntity(srcID, dstEntity);
    entityManager->removeEntity(dstID);
}

void Archetype::moveEntityRight(const EntityID eid, const ComponentID cid) {
    Archetype* intoArch = getRightNode(cid);
    ASSERT(hasEntity(eid), "Entity " << eid << " does not exist in Archetype " << hash << ".");
    ASSERT(intoArch != nullptr, "Right Archetype does not exist.");
    ASSERT(intoArch->hasComponent(cid), "Component " << cid << " does not exist on Right Archetype " << intoArch->getHash() << ".");

    Chunk* intoChunk = nullptr;
    Chunk* backChunk = nullptr;
    EntityRecord fromEntity = entityManager->getEntity(eid);
    EntityRecord intoEntity;
    EntityRecord backEntity;

    // allocate new chunk if no chunks exist in destination archetype
    if (intoArch->chunks.empty())
        intoArch->_allocateBackChunk();

    // allocate new chunk if back chunk is full in destination archetype
    intoChunk = intoArch->chunks.back();
    if (intoChunk->count == intoChunk->capacity) {
        intoArch->_allocateBackChunk();
        intoChunk = intoArch->chunks.back();
    }

    // add entity id to back of destination archetype
    intoArch->entities.push_back(eid);
    intoChunk->count++;

    // move entity component data to destination archetype
    intoEntity = EntityRecord{intoArch, intoChunk->index, intoChunk->count-1};
    copyEntityComponetData(fromEntity, intoEntity);

    // clear entity's added component data
    intoArch->clearComponentData(cid, intoEntity.col, intoEntity.row);

    // move entity component data from back to fill the missing spot in source archetype
    backChunk = chunks.back();
    backEntity = EntityRecord{this, backChunk->index, backChunk->count-1};
    copyEntityComponetData(backEntity, fromEntity);

    // move entity id from back index to the removed index
    size_t i = fromEntity.col * maxEntitiesPerChunk + fromEntity.row;
    this->entities[i] = this->entities.back();
    this->entities.pop_back();
    backChunk->count--;

    // deallocate back chunk if it is empty
    if (backChunk->count == 0)
        this->_deallocateBackChunk();

    // update entity manager
    entityManager->setEntity(eid, intoEntity);
}

void Archetype::moveEntityLeft(const EntityID eid, const ComponentID cid) {
    Archetype* intoArch = getLeftNode(cid);
    ASSERT(hasEntity(eid), "Entity " << eid << " does not exist in Archetype " << hash << ".");
    ASSERT(intoArch != nullptr, "Left Archetype does not exist.");

    Chunk* intoChunk = nullptr;
    Chunk* backChunk = nullptr;
    EntityRecord fromEntity = entityManager->getEntity(eid);
    EntityRecord intoEntity;
    EntityRecord backEntity;

    // allocate new chunk if no chunks exist in destination archetype
    if (intoArch->chunks.empty())
        intoArch->_allocateBackChunk();

    // allocate new chunk if back chunk is full in destination archetype
    intoChunk = intoArch->chunks.back();
    if (intoChunk->count == intoChunk->capacity) {
        intoArch->_allocateBackChunk();
        intoChunk = intoArch->chunks.back();
    }

    // add entity id to back of destination archetype
    intoArch->entities.push_back(eid);
    intoChunk->count++;

    // move entity component data to destination archetype
    intoEntity = EntityRecord{intoArch, intoChunk->index, intoChunk->count-1};
    copyEntityComponetData(fromEntity, intoEntity, true); // loop on destination component types

    // move entity component data from back to fill the missing spot in source archetype
    backChunk = chunks.back();
    backEntity = EntityRecord{this, backChunk->index, backChunk->count-1};
    copyEntityComponetData(backEntity, fromEntity);

    // move entity id from back index to the removed index
    size_t i = fromEntity.col * maxEntitiesPerChunk + fromEntity.row;
    this->entities[i] = this->entities.back();
    this->entities.pop_back();
    backChunk->count--;

    // deallocate back chunk if it is empty
    if (backChunk->count == 0)
        this->_deallocateBackChunk();

    // update entity manager
    entityManager->setEntity(eid, intoEntity);
}

// =============================================================================
// Graph Functions
// =============================================================================

Archetype* Archetype::getRightNode(const ComponentID id) {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        return nullptr;
    }
    return nodes[id].right;
}

Archetype* Archetype::getLeftNode(const ComponentID id) {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        return nullptr;
    }
    return nodes[id].left;
}

void Archetype::setRightNode(Archetype* arch, const ComponentID id) {
    nodes[id].right = arch;
}

void Archetype::setLeftNode(Archetype* arch, const ComponentID id) {
    nodes[id].left = arch;
}

// =============================================================================
// Chunk Component Data Accessor Functions
// =============================================================================

void* Archetype::getBufferLocation(const ComponentID id, const size_t col, const size_t row) {
    ASSERT(hasComponent(id), "Component " << id << " not in Archetype " << hash << ".");
    ASSERT(col < chunks.size(), "Requested col index out of bounds");
    ASSERT(row < chunks[col]->capacity, "Requested row index out of bounds");
    ComponentRecord component = components[id];
    Chunk* chunk = chunks[col];
    return reinterpret_cast<void*>(&chunk->buffer + component.offset + component.size * row);
}

template <typename T>
T* Archetype::getComponentDataArray(size_t col) {
    size_t id = typeof(T);
    ASSERT(hasComponent(id), "Component " << id << " not in Archetype " << hash << ".");
    ASSERT(col < chunks.size(), "Requested col index out of bounds");
    ComponentRecord component = components[id];
    Chunk* chunk = chunks[col];
    return reinterpret_cast<T*>(reinterpret_cast<void*>(&chunk->buffer + component.offset));
}

template <typename T>
T* Archetype::getComponentData(size_t col, size_t row) {
    size_t id = typeof(T);
    ASSERT(hasComponent(id), "Component " << id << " not in Archetype " << hash << ".");
    ASSERT(col < chunks.size(), "Requested col index out of bounds");
    ASSERT(row < chunks[col]->capacity, "Requested row index out of bounds");
    ComponentRecord component = components[id];
    Chunk* chunk = chunks[col];
    return reinterpret_cast<T*>(reinterpret_cast<void*>(&chunk->buffer + component.offset + component.size * row));
}

template <typename T>
void Archetype::clearComponentData(size_t col, size_t row) {
    size_t id = typeof(T);
    ASSERT(hasComponent(id), "Component " << id << " not in Archetype " << hash << ".");
    ASSERT(col < chunks.size(), "Requested col index out of bounds");
    ASSERT(row < chunks[col]->capacity, "Requested row index out of bounds");
    clearComponentData(id, col, row);
}

void Archetype::clearComponentData(ComponentID id, size_t col, size_t row) {
    ASSERT(hasComponent(id), "Component " << id << " not in Archetype " << hash << ".");
    ASSERT(col < chunks.size(), "Requested col index out of bounds");
    ASSERT(row < chunks[col]->capacity, "Requested row index out of bounds");
    ComponentRecord component = components[id];
    Chunk* chunk = chunks[col];
    std::memset(&chunk->buffer + component.offset + component.size * row, 0, component.size);
}

// =============================================================================
// Private Functions
// =============================================================================

void Archetype::_allocateBackChunk() {
    Chunk* chunk = (Chunk*)malloc(sizeof(Chunk) + Chunk::SIZE);
    chunk->index = chunks.size();
    chunk->count = 0;
    chunk->capacity = maxEntitiesPerChunk;
    chunk->arch = this;
    chunks.push_back(chunk);
}

void Archetype::_deallocateBackChunk() {
    free(chunks.back());
    chunks.pop_back();
}

// =============================================================================
// Inline Function
// =============================================================================

inline void copyEntityComponetData(
        const EntityRecord& src,
        const EntityRecord& dst,
        bool useDstSig
) {
    Archetype* loopArch = src.arch;
    if (useDstSig)
        loopArch = dst.arch;

    ComponentMap cmap = loopArch->getComponents();
    for (const ComponentID id : loopArch->getSignature()) {
        ComponentRecord c = cmap[id];
        size_t size = c.size;
        size_t offset = c.offset;
        void* srcPtr = src.arch->getBufferLocation(id, src.col, src.row);
        void* dstPtr = dst.arch->getBufferLocation(id, dst.col, dst.row);
        std::memcpy(dstPtr, srcPtr, size);
    }
}

} // namespace ECS