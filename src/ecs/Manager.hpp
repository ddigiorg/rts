// Manager.hpp
#pragma once

#include "ecs/Types.hpp"
#include "ecs/Archetype.hpp"
#include "utilities/Assert.hpp"

#include <iostream>
#include <queue>
#include <algorithm> // for std::sort
#include <type_traits> // for std::is_base_of
#include <cstring> // for std::memcpy

// TODO: smart pointers!

namespace ECS {

class Manager {
public:
    Manager();
    ~Manager();

    // checker functions
    bool hasEntity(const EntityID id);
    bool hasComponent(const ComponentID id);
    bool hasSystem(const SystemID id);
    bool hasEvent(EventID id);

    // registration functions
    template <typename C>
    void registerComponent();
    template<typename C>
    void registerTag();
    template <typename S>
    void registerSystem();
    template <typename E>
    void registerEvent();

    // event functions
    template <typename E, typename S>
    void subscribeToEvent();
    template <typename E>
    void triggerEvent();
    template <typename E>
    void queueEvent();
    void triggerQueuedEvents();

    // entity functions
    EntityID createEntity(const Signature& sig);
    void removeEntity(const EntityID id);
    void enableEntity(const EntityID id);
    void disableEntity(const EntityID id);
    bool isEntityEnabled(const EntityID id);
    template<typename T>
    void addEntityComponent(const EntityID id);
    template<typename T>
    void delEntityComponent(const EntityID id);
    // template<typename T>
    // void setEntityComponent(const EntityID id, T& value);
    // template<typename T>
    // T*   getEntityComponent(const EntityID id); // TODO: return reference instead of pointer?
    // template<typename T>
    // bool hasEntityComponent(const EntityID id);

    // singleton entity functions
    // template<typename T>
    // EntityID createSingleton();
    // template<typename T>
    // void removeSingleton();

    // query functions
    std::vector<Archetype*>& query(const Signature& all);

    void print();

private:
    EntityID _getNewEntityID();
    Archetype* _getOrCreateArchetype(const Signature& sig);

    // TODO: these suck and I want to kms myself... needs improvement
    void _moveEntityRight(const EntityID eid, const ComponentID cid);
    void _moveEntityLeft(const EntityID eid, const ComponentID cid);

    // TODO: make an entity manager class
    EntityMap entities = {};

    std::vector<ComponentRecord> components = {};
    std::vector<System*> systems = {};
    std::vector<EventRecord> events = {};

    std::unordered_map<Hash, Archetype*> archetypes = {};
    std::unordered_map<Hash, QueryRecord> queries = {};

    std::vector<EntityID> recycledEntityIDs;
    std::queue<EventID> eventQueue;
};

// =============================================================================
// Constructor and Destructor Functions
// =============================================================================

Manager::Manager() {
    registerTag<Disabled>();
}

Manager::~Manager() {
    for (System* system : systems) {
        if (system != nullptr)
            delete system;
    }

    // TODO: do I need to delete from entities too?
    for (const auto& pair : archetypes) {
        if (pair.second != nullptr)
            delete pair.second;
    }
}

// =============================================================================
// Checker Functions
// =============================================================================

bool Manager::hasEntity(const EntityID id) {
    if (entities.find(id) != entities.end())
        return true;
    return false;
}

bool Manager::hasComponent(const ComponentID id) {
    if (id < components.size())
        return true;
    return false;
}

bool Manager::hasSystem(const SystemID id) {
    if (id < systems.size())
        return true;
    return false;
}

bool Manager::hasEvent(const EventID id) {
    if (id < events.size())
        return true;
    return false;
}

// =============================================================================
// Registration Functions
// =============================================================================

template <typename C>
void Manager::registerComponent() {
    ComponentID id = getComponentID<C>();
    size_t size = sizeof(C);
    size_t offset = 0;
    // TODO: consider making STATIC_ASSERT macro so I can print the id
    static_assert(std::is_base_of<Component, C>::value, "C must inherit from Component.");
    ASSERT(!hasComponent(id), "Component " << id << " already exists.");
    components.push_back(ComponentRecord{size, offset});
}

template<typename C>
void Manager::registerTag() {
    ComponentID id = getComponentID<C>();
    size_t size = 0;
    size_t offset = 0;
    // TODO: consider making STATIC_ASSERT macro so I can print the id
    static_assert(std::is_base_of<Component, C>::value, "C must inherit from Component.");
    ASSERT(!hasComponent(id), "Component " << id << " already exists.");
    components.push_back(ComponentRecord{size, offset});
}

template <typename S>
void Manager::registerSystem() {
    SystemID id = getSystemID<S>();
    // TODO: consider making STATIC_ASSERT macro so I can print the id
    static_assert(std::is_base_of<System, S>::value, "S must inherit from System");
    ASSERT(!hasSystem(id), "System " << id << " already exists.");
    systems.emplace_back(new S());
    systems.back()->setManager(this);
}

template <typename E>
void Manager::registerEvent() {
    EventID id = getEventID<E>();
    // TODO: consider making STATIC_ASSERT macro so I can print the id
    static_assert(std::is_base_of<Event, E>::value, "E must inherit from Event.");
    ASSERT(!hasEvent(id), "Event " << id << " already exists.");
    events.push_back(EventRecord{{}});
}

// =============================================================================
// Event Functions
// =============================================================================

template <typename E, typename S>
void Manager::subscribeToEvent() {
    EventID eid = getEventID<E>();
    SystemID sid = getSystemID<S>();
    static_assert(std::is_base_of<Event, E>::value, "E must inherit from Event.");
    static_assert(std::is_base_of<System, S>::value, "S must inherit from System");
    ASSERT(hasEvent(eid), "Event " << eid << " does not exist.");
    ASSERT(hasSystem(sid), "System " << sid << " does not exist.");
    // TODO: need to detect if system is already in the event record systems vector
    events[eid].systems.push_back(systems[sid]);
}

template <typename E>
void Manager::triggerEvent() {
    EventID id = getEventID<E>();
    static_assert(std::is_base_of<Event, E>::value, "T must inherit from Event.");
    ASSERT(hasEvent(id), "Event " << id << " does not exist.");

    for (System* system : events[id].systems) {
        system->run();
    }
}

template <typename E>
void Manager::queueEvent() {
    EventID id = getEventID<E>();
    ASSERT(hasEvent(id), "Event " << id << " does not exist.");
    eventQueue.push(id);
}

void Manager::triggerQueuedEvents() {
    while(!eventQueue.empty()) {
        EventID id = eventQueue.front();
        ASSERT(hasEvent(id), "Event " << id << " does not exist.");

        for (System* system : events[id].systems) {
            system->run();
        }
        eventQueue.pop();
    }
}

// =============================================================================
// Entity Functions
// =============================================================================

// TODO: This is REALLY SLOW for ~1 million entities... NEEDS IMPROVEMENT
// TODO: should only happen on event before update
EntityID Manager::createEntity(const Signature& sig) {
    for (const ComponentID id : sig) {
        ASSERT(hasComponent(id), "Component " << id << " does not exist.");
    }
    Signature sortedSig = sig;
    std::sort(sortedSig.begin(), sortedSig.end());
    EntityID id = _getNewEntityID();
    Archetype* arch = _getOrCreateArchetype(sortedSig);
    EntityInfo einfo = EntityInfo{id, arch, 0, 0};
    arch->insertEntity(einfo);
    entities[einfo.id] = EntityRecord{einfo.arch, einfo.col, einfo.row};
    return id;
}

// TODO: this should only happen on event before update
void Manager::removeEntity(const EntityID id) {
    ASSERT(hasEntity(id), "Entity " << id <<" does not exist.");
    EntityRecord entity = entities[id];
    EntityInfo einfo = EntityInfo{id, entity.arch, entity.col, entity.row};
    einfo.arch->removeEntity(einfo);
    entities[einfo.id] = EntityRecord{einfo.arch, einfo.col, einfo.row};
    entities.erase(id);
}

void Manager::enableEntity(const EntityID id) {
    ASSERT(!isEntityEnabled(id), "Entity " << id << "is already enabled.");
    delEntityComponent<Disabled>(id);
}

void Manager::disableEntity(const EntityID id) {
    ASSERT(isEntityEnabled(id), "Entity " << id << "is already disabled.");
    addEntityComponent<Disabled>(id);
}

bool Manager::isEntityEnabled(const EntityID id) {
    ASSERT(hasEntity(id), "Entity " << id <<" does not exist.");
    Archetype* arch = entities[id].arch;
    bool hasDisabled = arch->hasComponent(typeof(Disabled));
    return !hasDisabled;
}

template<typename T>
void Manager::addEntityComponent(const EntityID eid) {
    ComponentID cid = getComponentID<T>();
    ASSERT(hasEntity(eid), "Entity " << eid << " does not exist.");
    ASSERT(hasComponent(cid), "Component " << cid << " does not exist.");
    Archetype* oldArch = entities[eid].arch;
    ASSERT(!oldArch->hasComponent(cid), "Entity " << eid << " already has Component " << cid << ".");

    _moveEntityRight(eid, cid);

    // TODO: perhaps this should be an Archetype function?
    // clear the entity's new component data
    EntityRecord& newEntity = entities[eid];
    Archetype* newArch = newEntity.arch;
    size_t newCol = newEntity.col;
    size_t newRow = newEntity.row;
    T* data = newArch->getComponentData<T>(newCol, newRow);
    *data = T{};
}

template<typename T>
void Manager::delEntityComponent(const EntityID eid) {
    ComponentID cid = getComponentID<T>();
    ASSERT(hasEntity(eid), "Entity " << eid << " does not exist.");
    ASSERT(hasComponent(cid), "Component " << cid << " does not exist.");
    Archetype* oldArch = entities[eid].arch;
    ASSERT(oldArch->hasComponent(cid), "Entity " << eid << " does not have Component " << cid << ".");

    _moveEntityLeft(eid, cid);
}

// template<typename T>
// void Manager::setEntityComponent(const EntityID id, T& value) {
//     size_t cid = getComponentID<T>();
//     ASSERT(hasEntity(id), "Entity " << id << " does not exist.");
//     ASSERT(hasComponent(cid), "Component " << cid << " does not exist.");
//     Entity& entity = entities[id];
//     if (!entity.arch->hasComponent(cid)) {
//         MoveEntityRight(id, cid);
//     }
//     entity.chunk->set(entity.index, value);
// }

// template<typename T>
// T* Manager::getEntityComponent(const EntityID id) {
//     size_t cid = getComponentID<T>();
//     ASSERT(hasEntity(id), "Entity " << id << " does not exist.");
//     ASSERT(hasComponent(cid), "Component " << cid << " does not exist.");
//     Entity& entity = entities[id];
//     return entity.chunk->get<T>(entity.index);
// }

// template<typename T>
// bool Manager::hasEntityComponent(const EntityID id) {
//     size_t cid = getComponentID<T>();
//     ASSERT(hasEntity(id), "Entity " << id << " does not exist.");
//     ASSERT(hasComponent(cid), "Component " << cid << " does not exist.");
//     Entity& entity = entities[id];
//     return entity.arch->hasComponent(cid);
// }

// =============================================================================
// Singleton Functions
// =============================================================================

// template<typename T>
// EntityID Manager::createSingleton() {
//     ComponentID cid = getComponentID<T>();
//     ASSERT(!hasEntity(cid), "Entity already exists: id=" << cid);
//     ASSERT(hasComponent(cid), "Component does not exist: id=" << cid);
//     Signature signature{cid};
//     Archetype* arch = _getOrCreateArchetype(signature);
//     entities[cid] = arch->insertEntity(cid);
//     return cid;
// }

// template<typename T>
// void Manager::removeSingleton() {
//     ComponentID cid = getComponentID<T>();
//     ASSERT(hasEntity(eid), "Entity does not exist: id=" << eid);
//     ASSERT(hasComponent(cid), "Component does not exist: id=" << cid);
//     // TODO: archetype of 1 chunk will still perist... delete it?
//     entities.erase(cid);
// }

// =============================================================================
// Query Functions
// =============================================================================

std::vector<Archetype*>& Manager::query(const Signature& all) {
    Signature allSorted = all;
    std::sort(allSorted.begin(), allSorted.end());
    Hash hash = hashSignature(allSorted);

    // if query already exists then return it
    auto it = queries.find(hash);
    if (it != queries.end())
        return it->second.matchingArchetypes;

    // search for match
    queries[hash] = {};
    std::vector<Archetype*>& matchingArchetypes = queries[hash].matchingArchetypes;

    for (const auto& pair : archetypes) {
        Archetype* archetype = pair.second;
        if (archetype->getHash() == hash)
            matchingArchetypes.push_back(archetype);
    }
    return matchingArchetypes;
}

// =============================================================================
// Print Function
// =============================================================================

void Manager::print() {

    std::cout << "Entities:" << std::endl;
    for (const auto& pair : entities) {
        const ID& id = pair.first;
        const EntityRecord& e = pair.second;
        std::cout << "  - id: " << id << std::endl;
        std::cout << "    arch: " << e.arch->getHash() << std::endl;
        std::cout << "    col: " << e.col << std::endl;
        std::cout << "    row: " << e.row << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Components:" << std::endl;
    for (size_t id = 0; id < components.size(); id++) {
        const ComponentRecord& component = components[id];
        std::cout << "  - id: " << id << std::endl;
        std::cout << "    size: " << component.size << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Systems:" << std::endl;
    for (size_t id = 0; id < systems.size(); id++) {
        std::cout << "  - id: " << id << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Events:" << std::endl;
    for (size_t id = 0; id < events.size(); id++) {
        const EventRecord& event = events[id];
        std::cout << "  - id: " << id << std::endl;
        std::cout << "  - numSystems: " << event.systems.size() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Archetypes:" << std::endl;
    for (const auto& pair : archetypes) {
        const Hash& hash = pair.first;
        Archetype* a = pair.second;
        std::cout << "  - hash: " << a->getHash() << std::endl;
        std::cout << "    signature:" << std::endl;
        for (ComponentID id : a->getSignature()) {
            std::cout << "    - id: " << id << std::endl;
        }
        std::cout << "    numEntities: " << a->getNumEntities() << std::endl;
        std::cout << "    numChunks: " << a->getNumChunks() << std::endl;
    }
    std::cout << std::endl;
}

// =============================================================================
// Private Functions
// =============================================================================

EntityID Manager::_getNewEntityID() {
    EntityID id = 0;
    if (recycledEntityIDs.size() > 0) {
        id = recycledEntityIDs.back();
        recycledEntityIDs.pop_back();
    }
    else {
        id = getNextEntityID();
    }
    return id;
}

Archetype* Manager::_getOrCreateArchetype(const Signature& sig) {
    Hash hash = hashSignature(sig);
    auto it = archetypes.find(hash);
    if (it == archetypes.end()) {
        auto result = archetypes.emplace(
            hash, new Archetype(hash, sig, components)
        );
        return result.first->second;
    }
    return it->second;
}

void Manager::_moveEntityRight(const EntityID eid, const ComponentID cid) {
    EntityRecord srcEntity = entities[eid];
    Archetype* srcArch = srcEntity.arch;
    Archetype* dstArch = srcArch->getRightNode(cid);

    // if destination Archetype does not exist then create it
    if (dstArch == nullptr) {

        // add component id to signature
        Signature sig = srcArch->getSignature();
        sig.push_back(cid);
        std::sort(sig.begin(), sig.end());

        // get or create destination archetype and set graph edges
        dstArch = _getOrCreateArchetype(sig);
        srcArch->setRightNode(dstArch, cid);
        dstArch->setLeftNode(srcArch, cid);
    }

    // insert entity into destination Archetype
    EntityInfo dstInfo = EntityInfo{eid, dstArch, 0, 0};
    dstArch->insertEntity(dstInfo);
    EntityRecord dstEntity = EntityRecord{dstInfo.arch, dstInfo.col, dstInfo.row};

    // copy entity's component data from source chunk to destination chunk
    Archetype::copyComponentData(
        srcArch->getSignature(),
        srcArch->getComponents(),
        srcEntity,
        dstEntity
    );

    // remove entity from source Archetype
    EntityInfo srcInfo = EntityInfo{eid, srcArch, srcEntity.col, srcEntity.row};
    srcArch->removeEntity(srcInfo);

    // update entities map
    entities[srcInfo.id] = EntityRecord{srcInfo.arch, srcInfo.col, srcInfo.row};
    entities[dstInfo.id] = EntityRecord{dstInfo.arch, dstInfo.col, dstInfo.row};
}

void Manager::_moveEntityLeft(const EntityID eid, const ComponentID cid) {
    EntityRecord srcEntity = entities[eid];
    Archetype* srcArch = srcEntity.arch;
    Archetype* dstArch = srcArch->getLeftNode(cid);

    // if destination Archetype does not exist then create it
    if (dstArch == nullptr) {

        // remove component id from signature
        Signature sig = srcArch->getSignature();
        auto newEnd = std::remove(sig.begin(), sig.end(), cid);
        sig.erase(newEnd, sig.end());
        std::sort(sig.begin(), sig.end());

        // get or create destination archetype and set graph edges
        dstArch = _getOrCreateArchetype(sig);
        srcArch->setLeftNode(dstArch, cid);
        dstArch->setRightNode(srcArch, cid);
    }

    // insert entity into destination Archetype
    EntityInfo dstInfo = EntityInfo{eid, dstArch, 0, 0};
    dstArch->insertEntity(dstInfo);
    EntityRecord dstEntity = EntityRecord{dstInfo.arch, dstInfo.col, dstInfo.row};

    // copy entity's component data from source chunk to destination chunk
    Archetype::copyComponentData(
        dstArch->getSignature(),
        dstArch->getComponents(),
        srcEntity,
        dstEntity
    );

    // remove entity from source Archetype
    EntityInfo srcInfo = EntityInfo{eid, srcArch, srcEntity.col, srcEntity.row};
    srcArch->removeEntity(srcInfo);

    // update entities map
    entities[srcInfo.id] = EntityRecord{srcInfo.arch, srcInfo.col, srcInfo.row};
    entities[dstInfo.id] = EntityRecord{dstInfo.arch, dstInfo.col, dstInfo.row};
}

} // namespace ECS