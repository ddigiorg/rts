#include "archetype.hpp"
#include "chunk.hpp"

#include <cassert>   // assert
#include <cstring>   // std::memcpy
#include <algorithm> // std::find

namespace ECS {

Archetype::Archetype(const SignatureType type, const Signature signature, ComponentMap& components, World* world) {
    size_t capacity = Chunk::BUFFER_SIZE;
    size_t entitySize = 0;
    size_t offset = 0;

    for (const ComponentID& cid : signature) {
        entitySize += components[cid].size;
    }

    assert(entitySize < Chunk::BUFFER_SIZE);

    if (entitySize > 0) {
        capacity = (size_t)(Chunk::BUFFER_SIZE / entitySize);
    }

    this->type = type;
    this->signature = signature;
    this->capacity = capacity;
    this->world = world;

    for (const ComponentID& cid : signature) {
        size_t size = components[cid].size;
        this->components[cid] = Component{size, offset};
        offset += capacity * size;
    }

    PushBackChunk();
}

Archetype::~Archetype() {
    for (Chunk* chunk : chunks) {
        delete chunk;
    }
}

bool Archetype::HasEntity(const EntityID id) {
    if (std::find(entities.begin(), entities.end(), id) != entities.end()) {
        return true;
    }
    return false;
}

bool Archetype::HasComponent(ComponentID id) {
    if (components.find(id) != components.end()) {
        return true;
    }
    return false;
}

bool Archetype::HasComponents(std::vector<ComponentID> cids) {
    for (const ComponentID& cid : cids) {
        if (!HasComponent(cid)) {
            return false;
        }
    }
    return true;
}

Entity Archetype::InsertEntity(const EntityID id) {
    assert(!HasEntity(id));

    if(chunks.back()->count == capacity) {
        PushBackChunk();
    }

    entities.push_back(id);

    Chunk* chunk = chunks.back();
    size_t index = chunk->count++;

    return Entity{id, this, chunk, index};
}

Entity Archetype::RemoveFromChunk(Chunk* chunk, const size_t index) {
    assert(chunks[0]->count > 0);
    // TODO: need any assertions?

    EntityID lastID = entities.back();

    Chunk* remvChunk = chunk;
    Chunk* lastChunk = chunks.back();
    size_t remvIndex = index;
    size_t lastIndex = lastChunk->count - 1;

    for (const ComponentID& cid : signature) {
        Component& component = components[cid];
        size_t size = component.size;
        size_t offset = component.offset;
        void* dstPtr = remvChunk->GetBufferLocation(cid, remvIndex);
        void* srcPtr = lastChunk->GetBufferLocation(cid, lastIndex);
        std::memcpy(dstPtr, srcPtr, size);
    }

    size_t i = chunk->chunkArrayIndex * capacity + remvIndex;
    entities[i] = lastID;
    entities.pop_back();

    lastChunk->count--;

    if (chunks.size() > 1 && lastChunk->count == 0) {
        PopBackChunk();
    }

    return Entity{lastID, this, chunk, index};
}

Archetype* Archetype::GetRightNode(const ComponentID id) {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        return nullptr;
    }
    return nodes[id].right;
}

Archetype* Archetype::GetLeftNode(const ComponentID id) {
    auto it = nodes.find(id);
    if (it == nodes.end()) {
        return nullptr;
    }
    return nodes[id].left;
}

void Archetype::SetRightNode(Archetype* arch, const ComponentID id) {
    nodes[id].right = arch;
}

void Archetype::SetLeftNode(Archetype* arch, const ComponentID id) {
    nodes[id].left = arch;
}

void Archetype::PushBackChunk() {
    Chunk* chunk = (Chunk*)malloc(Chunk::SIZE);
    chunk->world = world;
    chunk->arch = this;
    chunk->chunkArrayIndex = chunks.size();
    chunk->count = 0;
    chunks.push_back(chunk);
}

void Archetype::PopBackChunk() {
    free(chunks.back());
    chunks.pop_back();
}

Chunk* Archetype::GetChunk(size_t c) {
    assert(c < chunks.size());
    return chunks[c];
}

} // namespace ECS