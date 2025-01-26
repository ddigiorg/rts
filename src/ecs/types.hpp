// Types.hpp
#pragma once

#include <vector>
#include <unordered_map>

namespace ECS {

#define typeof(T) getComponentID<T>()

class Archetype;
class Manager;

using ID = size_t;
using EntityID = ID;
using ComponentID = ID;
using SystemID = ID;
using EventID = ID;

using Hash = size_t;
using Signature = std::vector<ComponentID>;

struct Event {};
struct Component {};
struct Disabled : Component {}; // Built in tag component for disabling entities

class System {
public:
    virtual void run() = 0;
    virtual ~System() = default;
    void setManager(Manager* ecs) { this->ecs = ecs; };
protected:
    Manager* ecs;
};

// TODO: sizeof this might be too large with a bunch of entities...
//       try smaller types than size_t?
struct EntityRecord {
    Archetype* arch; // Archetype pointer
    size_t col;      // index of a chunk in Archetype
    size_t row;      // index in the chunk
};

struct ComponentRecord {
    size_t size;   // size of component data
    size_t offset; // chunk buffer offset used in Archetype class
};

struct EventRecord {
    std::vector<System*> systems;
};

struct QueryRecord {
    std::vector<Archetype*> matchingArchetypes;
};



// TODO: might be worth figuring out how to make these vectors instead of maps for speed
using EntityMap = std::unordered_map<EntityID, EntityRecord>;




inline EntityID getNextEntityID() {
    static EntityID counter = 0;
    return counter++;
}

inline ComponentID getNextComponentID() {
    static ComponentID counter = 0;
    return counter++;
}

inline SystemID getNextSystemID() {
    static SystemID counter = 0;
    return counter++;
}

inline EventID getNextEventID() {
    static EventID counter = 0;
    return counter++;
}

template <typename T>
inline ComponentID getComponentID() {
    static ComponentID id = getNextComponentID();
    return id;
}

template <typename T>
inline SystemID getSystemID() {
    static SystemID id = getNextSystemID();
    return id;
}

template <typename T>
inline EventID getEventID() {
    static EventID id = getNextEventID();
    return id;
}

inline Hash hashSignature(const Signature& sig) {
    Hash hash = sig.size();
    for (ID id : sig) {
        hash ^= id + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

} // namespace ECS