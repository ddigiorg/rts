// Archetype.hpp
#pragma once

#include "ecs/Types.hpp"
#include "utilities/Assert.hpp"

#include <vector>
#include <unordered_map>
#include <cstring> // for std::memcpy
#include <algorithm> // for std::find

namespace ECS {

using ComponentMap = std::unordered_map<ComponentID, ComponentRecord>;

// A Chunk represents a contiguous block of memory for storing entiy component 
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

// TODO: this needs to be improved or removed... basically I needed a way to
//       edit the Manager EntityMap after inserting or removing entities from
//       an Archetype
struct EntityInfo {
    EntityID id;
    Archetype* arch;
    size_t col;
    size_t row;
};

struct ArchetypeGraphNode {
    Archetype* left;
    Archetype* right;
};

class Archetype {
public:
    Archetype(
        const Hash hash,
        const Signature& sig,
        const std::vector<ComponentRecord>& registeredComponents
    );

    ~Archetype();

    // checker functions
    bool hasEntity(const EntityID id);
    bool hasComponent(const ComponentID id);
    bool hasComponents(const Signature& sig);

    // TODO: using einfo is very confusing... need a better way to do this
    // entity functions
    void insertEntity(EntityInfo& einfo);
    void removeEntity(EntityInfo& einfo);

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

    // static function to copy component data from one Archetype to another
    static void copyComponentData(
        const Signature& componendIDs,
        const ComponentMap& cmap,
        const EntityRecord& src,
        const EntityRecord& dst
    );

    // getters
    Hash getHash() const { return hash; };
    const Signature& getSignature() const { return signature; };
    size_t getNumEntities() const { return entities.size(); };
    size_t getNumChunks() const { return chunks.size(); };
    size_t getChunkNumEntities(size_t c) const { return chunks[c]->count; };
    size_t getChunkMaxEntities(size_t c) const { return chunks[c]->capacity; };
    const ComponentMap& getComponents() const { return components; };

private:
    void _pushBackChunk();
    void _popBackChunk();

    Hash hash;
    Signature signature;
    size_t maxEntitiesPerChunk;
    std::vector<EntityID> entities;
    std::vector<Chunk*> chunks;
    ComponentMap components;
    std::unordered_map<ComponentID, ArchetypeGraphNode> nodes;
};

// =============================================================================
// Constructor and Destructor Functions
// =============================================================================

Archetype::Archetype(
        const Hash hash,
        const Signature& sig,
        const std::vector<ComponentRecord>& registeredComponents
) {
    size_t entitySize = 0;
    size_t offset = 0;

    this->hash = hash;
    this->signature = sig;

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
        // TODO: need to check if cid is in cmap
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
    if (std::find(entities.begin(), entities.end(), id) != entities.end())
        return true;
    return false;
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

void Archetype::insertEntity(EntityInfo& einfo) {
    ASSERT(!hasEntity(einfo.id), "Entity " << einfo.id << "is already in Archetype " << hash << ".");
    size_t id = einfo.id;

    // allocate a new chunk if there are no chunks or back chunk is full
    if (chunks.size() == 0 || chunks.back()->count == maxEntitiesPerChunk)
        _pushBackChunk();

    // add entity to back index of back chunk
    entities.push_back(id);

    einfo.col = chunks.size() - 1;
    einfo.row = chunks.back()->count++;
}

void Archetype::removeEntity(EntityInfo& einfo) {
    ASSERT(hasEntity(einfo.id), "Entity " << einfo.id << " does not exist in Archetype " << hash << ".");

    // get source and destination information
    Chunk* srcChunk = chunks.back();
    EntityID srcID = entities.back();
    EntityID dstID = einfo.id;
    EntityRecord dstEntity = EntityRecord{this, einfo.col, einfo.row};
    EntityRecord srcEntity = EntityRecord{this, chunks.size()-1, srcChunk->count-1};

    // copy buffer data from back entity to the entity being removed
    copyComponentData(signature, components, srcEntity, dstEntity);

    // copy entity id from back index to remove index
    size_t i = dstEntity.col * maxEntitiesPerChunk + dstEntity.row;
    entities[i] = srcID;
    entities.pop_back();
    srcChunk->count--;

    // deallocate back chunk if it is empty
    if (srcChunk->count == 0)
        _popBackChunk();

    einfo.id = srcID;
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

void Archetype::copyComponentData(
    const Signature& componendIDs,
    const ComponentMap& cmap,
    const EntityRecord& src,
    const EntityRecord& dst
) {
    for (const ComponentID id : componendIDs) {
        ComponentRecord c = cmap.at(id);
        size_t size = c.size;
        size_t offset = c.offset;
        void* srcPtr = src.arch->getBufferLocation(id, dst.col, dst.row);
        void* dstPtr = dst.arch->getBufferLocation(id, src.col, src.row);
        std::memcpy(dstPtr, srcPtr, size);
    }
}

// =============================================================================
// Private Functions
// =============================================================================

void Archetype::_pushBackChunk() {
    Chunk* chunk = (Chunk*)malloc(sizeof(Chunk) + Chunk::SIZE);
    chunk->index = chunks.size();
    chunk->count = 0;
    chunk->capacity = maxEntitiesPerChunk;
    chunk->arch = this;
    chunks.push_back(chunk);
}

void Archetype::_popBackChunk() {
    free(chunks.back());
    chunks.pop_back();
}

} // namespace ECS