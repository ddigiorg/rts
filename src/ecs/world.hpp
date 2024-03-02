#pragma once

#include "types.hpp"
#include "chunk.hpp"
#include "archetype.hpp"

#include <cassert> // assert
#include <queue>   // std::queue

namespace ECS {

class World {
public:
    World();
    ~World();

    bool HasEntity(const EntityID id);
    bool HasComponent(const ComponentID id);
    bool HasSystem(SystemID id);
    bool HasEvent(EventID id);

    template<typename T>
    ComponentID RegisterComponent();
    template<typename T>
    ComponentID RegisterTag();
    SystemID RegisterSystem(Function func, std::vector<ComponentID> cids);
    EventID RegisterEvent(std::vector<SystemID> sids);

    EntityID CreateEntity(); // TODO: figure out variadic templates
    void RemoveEntity(const EntityID eid);
    void EnableEntity(const EntityID eid);
    void DisableEntity(const EntityID eid);
    bool IsEnabled(const EntityID eid);
    template<typename T>
    void Add(const EntityID eid);
    template<typename T>
    void Del(const EntityID eid);
    template<typename T>
    void Set(const EntityID eid, T& value);
    template<typename T>
    T*   Get(const EntityID eid); // TODO: return reference instead of pointer?
    template<typename T>
    bool Has(const EntityID eid);

    template<typename T>
    EntityID CreateSingleton();
    template<typename T>
    void RemoveSingleton();
    template<typename T>
    void SetSingleton(T& value);
    template<typename T>
    T*   GetSingleton();
    template<typename T>
    bool HasSingleton();

    void SendEvent(EventID id);
    void ProcessEvent(EventID id);
    void ProcessEvents();

    // TODO:
    // - Need to be able to query for entities based on boolean logic (e.g. has X, Y but not Z components)
    // - This should return some sort of iterator that locates all chunks across matching archetypes
    // - Eventually this will be used in the underlying ProcessEvent to handle systems
    // Iterator* Query();

private:
    Archetype* GetOrCreateArchetype(Signature signature);
    void MoveEntityRight(const EntityID eid, const ComponentID cid);
    void MoveEntityLeft(const EntityID eid, const ComponentID cid);

    // std::vector<ID> recycledIDs; // TODO: need to recycle deleted ids

    EntityMap entities;
    ComponentMap components;
    SystemMap systems;
    EventMap events;
    ArchetypeMap archetypes;
    std::queue<EventID> eventQueue;
    ID disabledID;
};

template<typename T>
inline ComponentID World::RegisterComponent() {
    ComponentID id = GetComponentID<T>();
    assert(!HasComponent(id));
    size_t size = sizeof(T);
    components[id] = Component{size, NULL};
    return id;
}

template<typename T>
inline ComponentID World::RegisterTag() {
    ComponentID id = GetComponentID<T>();
    assert(!HasComponent(id));
    size_t size = 0; // NOTE: sizeof(T) when structure is empty is 1 not 0 so we force to 0
    components[id] = Component{size, NULL};
    return id;
}

template<typename T>
void World::Add(const EntityID eid) {
    ComponentID cid = GetComponentID<T>();
    assert(HasEntity(eid));
    assert(HasComponent(cid));
    Entity& entity = entities[eid];
    if (!entity.arch->HasComponent(cid)) {
        MoveEntityRight(eid, cid);
        entity.chunk->Set(entity.index, T{});
    }
}

template<typename T>
void World::Del(const EntityID eid) {
    ComponentID cid = GetComponentID<T>();
    assert(HasEntity(eid));
    assert(HasComponent(cid));
    Entity& entity = entities[eid];
    if (entity.arch->HasComponent(cid)) {
        MoveEntityLeft(eid, cid);
    }
}

template<typename T>
void World::Set(const EntityID eid, T& value) {
    ComponentID cid = GetComponentID<T>();
    assert(HasEntity(eid));
    assert(HasComponent(cid));
    Entity& entity = entities[eid];
    if (!entity.arch->HasComponent(cid)) {
        MoveEntityRight(eid, cid);
    }
    entity.chunk->Set(entity.index, value);
}

template<typename T>
T* World::Get(const EntityID eid) {
    ComponentID cid = GetComponentID<T>();
    assert(HasEntity(eid));
    assert(HasComponent(cid));
    Entity& entity = entities[eid];
    return entity.chunk->Get<T>(entity.index);
}

template<typename T>
bool World::Has(const EntityID eid) {
    ComponentID cid = GetComponentID<T>();
    assert(HasEntity(eid));
    assert(HasComponent(cid));
    Entity& entity = entities[eid];
    return entity.arch->HasComponent(cid);
}

template<typename T>
EntityID World::CreateSingleton() {
    ComponentID cid = GetComponentID<T>();
    assert(HasComponent(cid));
    assert(!HasEntity(cid));
    Signature signature{cid};
    Archetype* arch = GetOrCreateArchetype(signature);
    entities[cid] = arch->InsertEntity(cid);
    return cid;
}

template<typename T>
void World::RemoveSingleton() {
    ComponentID cid = GetComponentID<T>();
    assert(HasComponent(cid));
    assert(HasEntity(cid));
    // TODO: archetype of 1 chunk will still perist... delete it?
    entities.erase(cid);
}

template<typename T>
void World::SetSingleton(T& value) {
    ComponentID cid = GetComponentID<T>();
    assert(HasComponent(cid));
    assert(HasEntity(cid));
    Entity& entity = entities[cid];
    entity.chunk->Set(entity.index, value);
}

template<typename T>
T* World::GetSingleton() {
    ComponentID cid = GetComponentID<T>();
    assert(HasComponent(cid));
    assert(HasEntity(cid));
    Entity& entity = entities[cid];
    return entity.chunk->Get<T>(entity.index);
}

template<typename T>
bool World::HasSingleton() {
    ComponentID cid = GetComponentID<T>();
    assert(HasComponent(cid));
    assert(HasEntity(cid));
    Entity& entity = entities[cid];
    return entity.arch->HasComponent(cid);
}

} // namespace ECS