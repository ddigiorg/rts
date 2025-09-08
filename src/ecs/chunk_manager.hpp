#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/chunk.hpp"
#include "ecs/chunk_list.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "ecs/entity_manager.hpp"
#include "utils/assert.hpp"

#include <deque>
#include <unordered_map>
#include <vector>

namespace ECS {

// =============================================================================
// ChunkManager
// =============================================================================

class ChunkManager {
public:
    ChunkManager(ArchetypeManager& archetypeMgr, EntityManager& entityMgr);

    // access
    bool   hasChunk(ChunkID id) const;
    Chunk& getChunk(ChunkID id);

    

    // const ChunkList* getChunkList(ArchetypeMask a, GroupID g);

    // entity functions
    template<typename... Components>
    void insertEntity(EntityID eID, ArchetypeID aID, GroupID gID, Components&&... data);
    void removeEntity(EntityID eID);
    // void moveEntityToGroup()
    // void insertEntityComponent();
    // void removeEntityComponent();

    // miscellaneous
    void print();

private:
    // chunk management
    Chunk* _newChunk(GroupID gID, Archetype& archetype);
    void   _freeChunk(ChunkID cID);

    // chunk list management
    ChunkList* _getChunkList(GroupID gID, Archetype& archetype);
    ChunkList* _getOrCreateChunkList(GroupID gID, Archetype& archetype);

    // manager references
    ArchetypeManager& archetypeMgr;
    EntityManager& entityMgr;

    // chunk storage, empty chunks recycled for later reuse
    std::deque<Chunk>    chunks;
	std::vector<ChunkID> chunkFreeIDs;
    std::unordered_map<ChunkListKey, ChunkList, ChunkListHasher> lists;
};

// =============================================================================
// ChunkManager Constructor
// =============================================================================

ChunkManager::ChunkManager(
    ArchetypeManager& archetypeMgr,
    EntityManager& entityMgr)
        : archetypeMgr(archetypeMgr),
          entityMgr(entityMgr),
          chunks({}),
          chunkFreeIDs({}),
          lists({}) {}

// =============================================================================
// ChunkManager Access
// =============================================================================

bool ChunkManager::hasChunk(ChunkID id) const {
    if (id < static_cast<ChunkID>(chunks.size()))
        return !(chunks[id].getChunkID() == CHUNK_ID_NULL);
    return false;
}

Chunk& ChunkManager::getChunk(ChunkID id) {
    ASSERT(hasChunk(id), "ChunkID " << id << " does not exist.");
    return chunks[id];
}

// =============================================================================
// ChunkManager Entity Functions
// =============================================================================

template<typename... Components>
void ChunkManager::insertEntity(EntityID eID, ArchetypeID aID, GroupID gID, Components&&... data) {
    Archetype& archetype = archetypeMgr.getArchetype(aID);
    ChunkList* list = _getOrCreateChunkList(gID, archetype);
    Chunk* chunk = list->getNextOpenChunk();

    // allocate new chunk if no empty slots available
    if (chunk == nullptr) {
        chunk = _newChunk(gID, archetype);
        list->insertChunk(chunk);
        list->insertChunkOpen(chunk);
    }

    chunk->_insertEntity(eID, entityMgr, std::forward<Components>(data)...);

    // if chunk becomes full, remove it from open list
    if (chunk->isFull()) {
        list->removeChunkOpen(chunk);
    }
}

void ChunkManager::removeEntity(EntityID eID) {
    ChunkID cID = entityMgr.getEntity(eID).getChunkID();
    Chunk& chunk = getChunk(cID);

    chunk._removeEntity(eID, entityMgr);

    // TODO: clean this up (maybe put ptr to chunk list in chunk?)
    // Get chunk list for managing chunk state
    Archetype* archetype = chunk.getArchetype();
    ChunkListKey key{archetype->getMask(), chunk.getGroupID()};
    ChunkList& list = lists.at(key); // Should exist since chunk exists

    // if chunk becomes empty then remove it from lists and free it
    if (chunk.isEmpty()) {
        list.removeChunk(&chunk);
        list.removeChunkOpen(&chunk);
        _freeChunk(cID);
    }

    // if chunk is no longer full then add it back to open list
    else if (chunk.getCount() == chunk.getCapacity() - 1) {
        list.insertChunkOpen(&chunk);
    }
}

// =============================================================================
// ChunkManager Miscellaneous Functions
// =============================================================================

void ChunkManager::print() {
    std::cout << "chunks:" << std::endl;
    for (const Chunk& c : chunks) {
        if (c.getChunkID() == CHUNK_ID_NULL) continue;
        std::cout << "  - id: "       << c.getChunkID()  << std::endl;
        std::cout << "    group: "    << c.getGroupID()  << std::endl;
        std::cout << "    count: "    << c.getCount()    << std::endl;
        std::cout << "    capacity: " << c.getCapacity() << std::endl;
    }
}

// =============================================================================
// ChunkManager Private Functions
// =============================================================================

Chunk* ChunkManager::_newChunk(GroupID gID, Archetype& archetype) {
    ChunkID cID = CHUNK_ID_NULL;

    if (!chunkFreeIDs.empty()) {
        cID = chunkFreeIDs.back();
        chunkFreeIDs.pop_back();
        chunks[cID]._initialize(cID, gID, &archetype);
        return &chunks[cID];
    }

    cID = static_cast<ChunkID>(chunks.size());
    chunks.emplace_back();
    chunks.back()._initialize(cID, gID, &archetype);
    return &chunks[cID];
}

void ChunkManager::_freeChunk(ChunkID cID) {
    ASSERT(hasChunk(cID), "ChunkID " << cID << " does not exist.");
    chunks[cID]._clear();
    chunkFreeIDs.push_back(cID);
}

ChunkList* ChunkManager::_getChunkList(GroupID gID, Archetype& archetype) {
    ChunkListKey key{archetype.getMask(), gID};
    auto it = lists.find(key);
    return (it != lists.end()) ? &it->second : nullptr;
}

ChunkList* ChunkManager::_getOrCreateChunkList(GroupID gID, Archetype& archetype) {
    ChunkListKey key{archetype.getMask(), gID};
    auto it = lists.find(key);
    if (it == lists.end()) {
        return &lists.emplace(key, ChunkList(key)).first->second;
    }
    return &it->second;
}

} // namespace ECS