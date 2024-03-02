#pragma once

#include "types.hpp"

#include <vector>
#include <unordered_map>

namespace ECS {

struct ArchetypeGraphNode {
    Archetype* left;
    Archetype* right;
};

class Archetype {
public:
    friend class Chunk;

    Archetype(const SignatureType type, const Signature signature, ComponentMap& components, World* world);
    ~Archetype();
    bool HasEntity(const EntityID id);
    bool HasComponent(const ComponentID id);
    bool HasComponents(std::vector<ComponentID> ids);
    Entity InsertEntity(const EntityID id);
    Entity RemoveFromChunk(Chunk* chunk, const size_t index);
    Archetype* GetRightNode(const ComponentID id);
    Archetype* GetLeftNode(const ComponentID id);
    void SetRightNode(Archetype* arch, const ComponentID id);
    void SetLeftNode(Archetype* arch, const ComponentID id);
    void PushBackChunk();
    void PopBackChunk();
    Chunk* GetChunk(size_t c);
    size_t GetCapacity() { return capacity; };
    Signature GetSignatureCopy() { return signature; };
    ComponentMap& GetComponentMap() { return components; }; // TODO: should return constant so it doesnt change?
    std::vector<Chunk*>& GetChunks() { return chunks; }; // TODO: same here

private:
    size_t capacity;
    SignatureType type;
    Signature signature;
    ComponentMap components;
    std::vector<Chunk*> chunks;
    std::vector<EntityID> entities;
    std::unordered_map<ComponentID, ArchetypeGraphNode> nodes;
    World* world;
};

} // namespace ECS