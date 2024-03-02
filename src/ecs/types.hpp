#pragma once

#include <vector>        // std::vector
#include <unordered_map> // std::unordered_map
#include <functional>    // std::function

namespace ECS {

class Chunk;
class Archetype;
class World;

using ID = size_t;
using EntityID = ID;
using ComponentID = ID;
using SystemID = ID;
using EventID = ID;

using Function = std::function<void(Chunk*)>;

using Signature = std::vector<ComponentID>;
using SignatureType = size_t;

inline SignatureType HashSignature(const Signature& signature) {
    SignatureType hash = signature.size();
    for (ComponentID id : signature) {
        hash ^= id + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

inline ID& NextID() {
    static ID id = 1;
    return id;
}

template <typename T>
inline ComponentID GetComponentID() {
    static ComponentID cid = NextID()++;
    return cid;
}

struct Entity {
    EntityID id;
    Archetype* arch = nullptr;
    Chunk* chunk = nullptr;
    size_t index = NULL;
};

struct Component {
    size_t size = NULL;
    size_t offset = NULL;
};

struct System {
    Function func;
    Signature signature;
    std::vector<Archetype*> archs;
};

struct Event {
    std::vector<System*> systems;
};

using EntityMap = std::unordered_map<EntityID, Entity>;
using ComponentMap = std::unordered_map<ComponentID, Component>;
using SystemMap = std::unordered_map<SystemID, System>;
using EventMap = std::unordered_map<EventID, Event>;
using ArchetypeMap = std::unordered_map<SignatureType, Archetype*>;

struct Disabled{}; // Tag component for disabling entities

} // namespace ECS