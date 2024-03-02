#pragma once

#include "world.hpp"

#include <cassert>   // assert
#include <cstring>   // std::memcpy
#include <algorithm> // std::sort
#include <iostream>  // std::cout

namespace ECS {

World::World() {
    disabledID = RegisterTag<Disabled>();
}

World::~World() {
    for (const auto& pair : archetypes) {
        delete pair.second;
    }
}

bool World::HasEntity(const EntityID id) {
    if (entities.find(id) != entities.end()) {
        return true;
    }
    return false;
}

bool World::HasComponent(const ComponentID id) {
    if (components.find(id) != components.end()) {
        return true;
    }
    return false;
}

bool World::HasSystem(const SystemID id) {
    if (systems.find(id) != systems.end()) {
        return true;
    }
    return false;
}

bool World::HasEvent(const EventID id) {
    if (events.find(id) != events.end()) {
        return true;
    }
    return false;
}

SystemID World::RegisterSystem(Function func, std::vector<ComponentID> cids) {
    SystemID id = NextID()++;
    std::sort(cids.begin(), cids.end());
    systems[id] = System{func, cids};
    return id;
}

EventID World::RegisterEvent(std::vector<SystemID> sids) {
    EventID id = NextID()++;
    std::vector<System*> eventSystems;
    for(const SystemID& sid : sids) {
        assert(HasSystem(sid));
        eventSystems.push_back(&systems[sid]);
    }
    events[id] = Event{eventSystems};
    return id;
}

EntityID World::CreateEntity() {
    Signature signature{};
    Archetype* arch = GetOrCreateArchetype(signature);
    EntityID id = NextID()++;
    entities[id] = arch->InsertEntity(id);
    return id;
}

void World::RemoveEntity(const EntityID eid) {
    assert(HasEntity(eid));
    Entity& entity = entities[eid];
    Entity replacedEntity = entity.arch->RemoveFromChunk(entity.chunk, entity.index);
    entities[replacedEntity.id] = replacedEntity;
    entities.erase(eid);
}

void World::EnableEntity(const EntityID eid) {
    assert(!IsEnabled(eid));
    Del<Disabled>(eid);
}

void World::DisableEntity(const EntityID eid) {
    assert(IsEnabled(eid));
    Add<Disabled>(eid);
}

bool World::IsEnabled(const EntityID eid) {
    return !Has<Disabled>(eid);
}

Archetype* World::GetOrCreateArchetype(Signature signature) {
    std::sort(signature.begin(), signature.end());
    SignatureType type = HashSignature(signature);

    auto it = archetypes.find(type);
    if (it == archetypes.end()) {
        Archetype* arch = new Archetype(type, signature, components, this);
        archetypes[type] = arch;

        // update systems with pointer to the new archetype if applicable
        for (auto& spair : systems) {
            System* system = &spair.second;
            Signature systemSig = system->signature;

            if (arch->HasComponents(systemSig)) {
                system->archs.push_back(arch);
            }
        }

        return arch;
    }

    return archetypes[type];
}

void World::MoveEntityRight(const EntityID eid, const ComponentID cid) {
    Entity* entity = &entities[eid];
    Archetype* srcArch = entity->arch;
    Archetype* dstArch = srcArch->GetRightNode(cid);

    if (dstArch == nullptr) {
        Signature signature = srcArch->GetSignatureCopy();
        signature.push_back(cid);
        dstArch = GetOrCreateArchetype(signature);
        srcArch->SetRightNode(dstArch, cid);
        dstArch->SetLeftNode(srcArch, cid);
    }

    Entity insertedEntity = dstArch->InsertEntity(eid);

    Chunk* srcChunk = entity->chunk;
    Chunk* dstChunk = dstArch->GetChunks().back();
    size_t srcIndex = entity->index;
    size_t dstIndex = dstChunk->GetCount() - 1;

    // TODO: make this a function in Archetype?
    for (const ComponentID& cid : srcArch->GetSignatureCopy()) {
        Component& component = srcArch->GetComponentMap()[cid];
        size_t size = component.size;
        size_t offset = component.offset;
        void* dstPtr = dstChunk->GetBufferLocation(cid, dstIndex);
        void* srcPtr = srcChunk->GetBufferLocation(cid, srcIndex);
        std::memcpy(dstPtr, srcPtr, size);
    }

    Entity replacedEntity = srcArch->RemoveFromChunk(entity->chunk, entity->index);

    entities[replacedEntity.id] = replacedEntity;
    entities[insertedEntity.id] = insertedEntity;
}

void World::MoveEntityLeft(const EntityID eid, const ComponentID cid) {
    Entity* entity = &entities[eid];
    Archetype* srcArch = entity->arch;
    Archetype* dstArch = srcArch->GetLeftNode(cid);

    if (dstArch == nullptr) {
        Signature signature = srcArch->GetSignatureCopy();
        signature.push_back(cid);
        dstArch = GetOrCreateArchetype(signature);
        srcArch->SetLeftNode(dstArch, cid);
        dstArch->SetRightNode(srcArch, cid);
    }

    Entity insertedEntity = dstArch->InsertEntity(eid);

    Chunk* srcChunk = entity->chunk;
    Chunk* dstChunk = dstArch->GetChunks().back();
    size_t srcIndex = entity->index;
    size_t dstIndex = dstChunk->GetCount() - 1;

    for (const ComponentID& cid : dstArch->GetSignatureCopy()) {
        Component& component = dstArch->GetComponentMap()[cid];
        size_t size = component.size;
        size_t offset = component.offset;
        void* dstPtr = dstChunk->GetBufferLocation(cid, dstIndex);
        void* srcPtr = srcChunk->GetBufferLocation(cid, srcIndex);
        std::memcpy(dstPtr, srcPtr, size);
    }

    Entity replacedEntity = srcArch->RemoveFromChunk(entity->chunk, entity->index);

    entities[replacedEntity.id] = replacedEntity;
    entities[insertedEntity.id] = insertedEntity;
}

void World::SendEvent(EventID id) {
    assert(HasEvent(id));
    eventQueue.push(id);
}

void World::ProcessEvent(EventID id) {
    assert(HasEvent(id));
    Event* event = &events[id];

    // TODO: need to figure out how to filter disabled entities...
    for (System* system : event->systems) {
        Function func = system->func;
        for (Archetype* arch : system->archs) {
            for (Chunk* chunk : arch->GetChunks()) {
                func(chunk);
            }
        }
    }
}

void World::ProcessEvents() {
    while(!eventQueue.empty()) {
        EventID id = eventQueue.front();
        ProcessEvent(id);
        eventQueue.pop();
    }
}

} // namespace ECS