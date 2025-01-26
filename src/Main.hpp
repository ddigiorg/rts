// Main.hpp
#pragma once

#include "utilities/Assert.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm> // For std::sort

#define typeof(T) getID<T>()

struct Chunk;
class Archetype;

using ID = size_t;
using EntityID = ID;
using ComponentID = ID;

using Signature = std::vector<ComponentID>;
using SignatureHash = size_t;

inline SignatureHash hashSignature(const Signature& sig) {
    SignatureHash hash = sig.size();
    for (ID id : sig) {
        hash ^= id + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

inline ID generateID() {
    static ID counter = 1;
    return counter++;
}

template <typename T>
ID getID() {
    static ID id = generateID();
    return id;
}

struct EntityRecord {
    Archetype* arch; // Archetype pointer
    size_t col; // index of a chunk in Archetype
    size_t row; // index in the chunk
};

struct ComponentRecord {
    ComponentID id;
    size_t size;
    size_t offset;
};

using ComponentMap = std::unordered_map<ComponentID, ComponentRecord>;


// A Chunk represents a contiguous block of memory for storing components of
// entities in the ECS engine.  It is designed for efficient memory access and
// cache locality.  Each chunk is 16 kilobytes which is a good fit for most CPU
// L1 caches.  It is primarily managed by its associated Archetype.
struct Chunk {
    using byte = char;
    static const size_t SIZE = 16 * 1024 - 256; // 16 kilobytes minus header overhead
    size_t index;    // chunk number in the archetype
    size_t count;    // current number of entities in the chunk
    size_t capacity; // maximum number of entities in the chunk
    Archetype* arch; // pointer to the parent Archetype
    byte buffer;     // buffer start
};

// =============================================================================
// Archetype
// =============================================================================

class Archetype {
public:
    Archetype(const Signature& sig, const ComponentMap& cmap);
    ~Archetype();
    const EntityRecord& insertEntity(const EntityID& id);

    template <typename T>
    T* getChunkComponentDataArray(size_t col);

    template <typename T>
    T* getChunkComponentData(size_t col, size_t row);

    size_t getNumChunks() const { return chunks.size(); };
    size_t getChunkNumEntities(size_t c) const { return chunks[c]->count; };
    size_t getChunkMaxEntities(size_t c) const { return chunks[c]->capacity; };

private:
    void _pushBackChunk();
    void _popBackChunk();

    Signature signature;
    SignatureHash hash;
    size_t maxEntitiesPerChunk;
    std::unordered_map<ComponentID, ComponentRecord> components;
    std::vector<EntityID> entities;
    std::vector<Chunk*> chunks;
};

Archetype::Archetype(const Signature& sig, const ComponentMap& cmap) {
    Signature sortedSig = sig;
    size_t entitySize = 0;
    size_t offset = 0;

    // sort and store the signature
    std::sort(sortedSig.begin(), sortedSig.end());
    this->signature = sortedSig;
    this->hash = hashSignature(signature);

    // calculate size of entity data
    for (const ComponentID& id : signature) {
        // TODO: need to check if cid is in cmap
        entitySize += cmap.at(id).size;
    }
    ASSERT(entitySize < Chunk::SIZE, "Entity component data size larger than chunk buffer size!");

    // calculate numEntities based on chunk buffer size and size of entity data
    this->maxEntitiesPerChunk = Chunk::SIZE;
    if (entitySize > 0)
        this->maxEntitiesPerChunk = (size_t)(Chunk::SIZE / entitySize);

    // set components
    for (const ComponentID& id : signature) {
        // TODO: need to check if cid is in cmap
        size_t size = cmap.at(id).size;
        components.emplace(id, ComponentRecord{id, size, offset});
        offset += maxEntitiesPerChunk * size;
    }
}

Archetype::~Archetype() {
    for (Chunk* chunk : chunks) {
        if (chunk != nullptr)
            free(chunk);
    }
}

const EntityRecord& Archetype::insertEntity(const EntityID& id) {
    if (chunks.size() == 0 || chunks.back()->count == maxEntitiesPerChunk)
        _pushBackChunk();

    Chunk* chunk = chunks.back();
    size_t col = chunks.size() - 1;
    size_t row = chunk->count;

    entities.push_back(id);
    chunk->count++;

    return EntityRecord{this, col, row};
}

template <typename T>
T* Archetype::getChunkComponentDataArray(size_t col) {
    size_t id = typeof(T);
    ComponentRecord component = components[id];
    Chunk* chunk = chunks[col];
    return reinterpret_cast<T*>(reinterpret_cast<void*>(&chunk->buffer + component.offset));
}

template <typename T>
T* Archetype::getChunkComponentData(size_t col, size_t row) {
    size_t id = typeof(T);
    ComponentRecord component = components[id];
    Chunk* chunk = chunks[col];
    return reinterpret_cast<T*>(reinterpret_cast<void*>(&chunk->buffer + component.offset + component.size * row));
}

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

// =============================================================================
// QueryIterator
// =============================================================================

// // for (auto it = query.begin(); it != query.end(); ++it) {
// //     EntityID entity = it.getEntity();
// //     auto pos = it.getComponentData<Position>();
// //     auto vel = it.getComponentData<Velocity>();
// //     pos.x += vel.x;
// //     pos.y += vel.y;
// // }
// class QueryIterator {
// public:
//     QueryIterator(
//         const std::vector<Archetype*>& archetypes,
//         size_t table = 0,
//         size_t col = 0,
//         size_t row = 0
//     );

//     bool operator!=(const QueryIterator& other) const {
//         return table != other.table || col != other.col || row != other.row;
//     }

//     QueryIterator& operator++() {
//         ++row;
//         if (row >= currentArchetype->getChunkNumEntities()) {
//             ++col;
//             row = 0;
//             _update();
//         }

//         return *this;
//     }

//     EntityID getEntityID() const { return currentArchetype->getEntityID(row); };

//     // TODO: probably could be more efficient by somehow indexing off getComponentDataArray
//     template <typename T>
//     T* getComponentData() const { return currentArchetype->getComponentData<T>(col, row); };

// private:
//     void _update();

//     std::vector<Archetype*> archetypes;
//     size_t table; // index of archetype
//     size_t col;   // index of chunk in archetype
//     size_t row;   // index in chunk
// };

// QueryIterator::QueryIterator(
//         const std::vector<Archetype*>& archetypes,
//         size_t table = 0,
//         size_t col = 0,
//         size_t row = 0
// ) {
//     this->archetypes = archetypes;
//     this->table = table;
//     this->col = col;
//     this->row = row;
//     _update();
// }

// void QueryIterator::_update() {
//     while(table < archetypes.size()) {
//         const Archetype* archetype = archetypes[table];
//         if (col < archetype->getNumChunks()) {
//             return;
//         }
//         ++table;
//         col = 0;
//     }
// }



// int main() {

//     // // print chunk variable addresses
//     // Chunk* chunk = (Chunk*)malloc(sizeof(Chunk) + Chunk::BUFFER_SIZE);
//     // std::cout << reinterpret_cast<void*>(chunk) << std::endl;
//     // std::cout << reinterpret_cast<void*>(&chunk->index) << std::endl;
//     // std::cout << reinterpret_cast<void*>(&chunk->capacity) << std::endl;
//     // std::cout << reinterpret_cast<void*>(&chunk->prevChunk) << std::endl;
//     // std::cout << reinterpret_cast<void*>(&chunk->nextChunk) << std::endl;
//     // std::cout << reinterpret_cast<void*>(&chunk->buffer) << std::endl;
//     // free(chunk);

//     Signature signature = {};
//     ComponentMap components = {};

//     ComponentID id = 0;
//     size_t size = 0;
//     size_t offset = 0;

//     id = typeof(Position);
//     size = sizeof(Position);
//     components.emplace(id, ComponentRecord{id, size, offset});
//     signature.push_back(id);

//     id = typeof(Velocity);
//     size = sizeof(Velocity);
//     components.emplace(id, ComponentRecord{id, size, offset});
//     signature.push_back(id);

//     Archetype archetype = Archetype(signature, components);

//     archetype.insertEntity(0);
//     archetype.insertEntity(1);
//     archetype.insertEntity(2);

//     // Position* positions = archetype.getChunkComponentDataArray<Position>(0);
//     // Position* position0 = archetype.getChunkComponentData<Position>(0, 0);
//     // position0->x = 1.0f;
//     // position0->y = 1.0f;
//     // std::cout << positions[0].x << ", " << positions[0].y << std::endl;
//     // std::cout << positions[1].x << ", " << positions[1].y << std::endl;

//     // for archetype in query.getArchetypes()

//     // set initial values
//     auto pArr = archetype.getChunkComponentDataArray<Position>(0);
//     auto vArr = archetype.getChunkComponentDataArray<Velocity>(0);

//     pArr[0] = Position{0.0f, 0.0f};
//     pArr[1] = Position{0.0f, 0.0f};
//     pArr[2] = Position{0.0f, 0.0f};

//     vArr[0] = Velocity{0.5f, 1.0f};
//     vArr[1] = Velocity{1.5f, 2.0f};
//     vArr[2] = Velocity{2.5f, 3.0f};

//     // simulate System
//     std::vector<Archetype*> archetypes = {&archetype}; // = ecs.query(signature)

//     for (size_t j = 0; j < 2; j++) {

//         for (Archetype* archetype : archetypes) {
//             for (size_t c = 0; c < archetype->getNumChunks(); c++) {
//                 auto pos = archetype->getChunkComponentDataArray<Position>(c);
//                 auto vel = archetype->getChunkComponentDataArray<Velocity>(c);

//                 for (size_t i = 0; i < archetype->getChunkNumEntities(c); i++) {
//                     // EntityID = archetype->getEntityID(c, i);
//                     pos[i].x += vel[i].x;
//                     pos[i].y += vel[i].y;
//                 }
//             }
//         }

//     }

//     // print results
//     std::cout << pArr[0].x << ", " << pArr[0].y << std::endl;
//     std::cout << pArr[1].x << ", " << pArr[1].y << std::endl;
//     std::cout << pArr[2].x << ", " << pArr[2].y << std::endl;
//     std::cout << std::endl;
//     std::cout << vArr[0].x << ", " << vArr[0].y << std::endl;
//     std::cout << vArr[1].x << ", " << vArr[1].y << std::endl;
//     std::cout << vArr[2].x << ", " << vArr[2].y << std::endl;
//     std::cout << std::endl;
//     std::cout << archetype.getChunkMaxEntities(0) << std::endl;

//     return 0;
// }