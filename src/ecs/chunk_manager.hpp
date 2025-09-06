#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/chunk.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <deque>
#include <unordered_map>
#include <vector>

namespace ECS {

// =============================================================================
// ChunkManager Types
// =============================================================================

struct ChunkListKey {
    ArchetypeMask archetype;
    GroupID group;

    bool operator==(const ChunkListKey& other) const {
        return archetype == other.archetype && group == other.group;
    }
};

// TODO: look into these hashing techniques
// https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/
// https://github.com/matteo65/mzHash64

struct ChunkListHasher {
    size_t operator()(const ChunkListKey& key) const {
        size_t h1 = std::hash<ArchetypeMask>{}(key.archetype);
        size_t h2 = std::hash<GroupID>{}(key.group);

        // boost-style hash combine
        // return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // TODO: need 32 bit or 64?
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));       
    }
};

struct ChunkList {
    Chunk* headChunk = nullptr;     // list node to head chunk
	Chunk* tailChunk = nullptr;     // list node to tail chunk
	Chunk* headChunkOpen = nullptr; // list node to head chunk with open entity slots
	Chunk* tailChunkOpen = nullptr; // list node to tail chunk with open entity slots
};

// =============================================================================
// ChunkManager
// =============================================================================

class ChunkManager {
public:
    ChunkManager(ArchetypeManager& archetypeMgr, EntityManager& entityMgr);

    // queries
    bool hasChunk(ChunkID id) const;
    // bool hasChunkList();

    // getters
    Chunk& getChunk(ChunkID id);
    // const ChunkList* getChunkList(ArchetypeMask a, GroupID g);

    // entity functions
    template<typename... Components>
    void insertEntity(EntityID eID, ArchetypeID aID, GroupID gID, Components&&... data);
    // void removeEntity(EntityID eID);
    // void moveEntityToGroup()
    // void insertEntityComponent();
    // void removeEntityComponent();

    // miscellaneous
    void print();

private:
    // chunk management
    Chunk* _newChunk(Archetype& a, GroupID g);
    void   _freeChunk(ChunkID id);

    // chunk list management
    ChunkList* _getOrCreateChunkList(Archetype& a, GroupID g);
    void _insertChunkIntoList(Chunk* chunk, ChunkList* list);
    // void _removeChunkFromList(Chunk* c, ArchetypeMask a, GroupID g);
    void _removeChunkFromOpenList(Chunk* chunk, ChunkList* list);

    // entity helpers
    template<typename T, typename... Rest>
    void _setAllEntityComponentData(Chunk* chunk, ChunkIdx index, T&& first, Rest&&... rest);
    template<typename T>
    void _setOneEntityComponentData(Chunk* chunk, ChunkIdx index, T&& data);

    // manager references
    ArchetypeManager& archetypeMgr;
    EntityManager& entityMgr;

    // chunk storage, empty chunks recycled for later reuse
    std::deque<Chunk>    chunks;
	std::vector<ChunkID> chunkFreeIDs;
    std::unordered_map<ChunkListKey, ChunkList, ChunkListHasher> chunkLists;
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
          chunkLists({}) {}

// =============================================================================
// ChunkManager Queries
// =============================================================================

bool ChunkManager::hasChunk(ChunkID id) const {
    if (id < static_cast<ChunkID>(chunks.size()))
        return !(chunks[id].id == CHUNK_ID_NULL);
    return false;
}

// =============================================================================
// ChunkManager Getters
// =============================================================================

Chunk& ChunkManager::getChunk(ChunkID id) {
    ASSERT(hasChunk(id), "ChunkID " << id << " does not exist.");
    return chunks[id];
}

// const ChunkList* ChunkManager::getChunkList(ArchetypeMask archetype, GroupID group) const {
//     ChunkListKey key{archetype, group};
//     auto it = chunkLists.find(key);
//     return (it != chunkLists.end()) ? &it->second : nullptr;
// }

// =============================================================================
// ChunkManager Entity Functions
// =============================================================================

template<typename T, typename... Rest>
void ChunkManager::_setAllEntityComponentData(Chunk* chunk, ChunkIdx index, T&& first, Rest&&... rest) {
    // set the first component data
    _setOneEntityComponentData<T>(chunk, index, std::forward<T>(first));

    // recursively set the rest if any
    if constexpr (sizeof...(rest) > 0) {
        _setAllEntityComponentData(chunk, index, std::forward<Rest>(rest)...);
    }
}

template<typename T>
void ChunkManager::_setOneEntityComponentData(Chunk* chunk, ChunkIdx index, T&& data) {
    // only set data for non-tag components
    if constexpr (!IsTagType<T>) {
        ASSERT(chunk->hasComponent<T>(), "ERROR"); // TODO: error msg
        T* arr = chunk->data<T>();
        arr[index] = std::forward<T>(data);
    }
}

template<typename... Components>
void ChunkManager::insertEntity(EntityID eID, ArchetypeID aID, GroupID gID, Components&&... data) {
    Entity& entity = entityMgr.getEntity(eID);
    Archetype& archetype = archetypeMgr.getArchetype(aID);
    Chunk* chunk = nullptr;
    ChunkList* list = _getOrCreateChunkList(archetype, gID);

    // select the first open chunk otherwise allocate a new chunk
    if (list->headChunkOpen && !list->headChunkOpen->isFull()) {
        chunk = list->headChunkOpen;
    } else {
        chunk = _newChunk(archetype, gID);
        _insertChunkIntoList(chunk, list);
    }

    // insert entity id and component data into the chunk
    ChunkIdx index = chunk->count;
    chunk->_getEntityIDs()[index] = eID;
    _setAllEntityComponentData(chunk, index, std::forward<Components>(data)...);
    chunk->count++;
    // chunk->version++;

    // update entity's location
    entity._setLocation(chunk->id, index);

    // if chunk becomes full, remove it from open list
    if (chunk->isFull()) {
        _removeChunkFromOpenList(chunk, list);
    }
}

// =============================================================================
// ChunkManager Miscellaneous Functions
// =============================================================================

void ChunkManager::print() {
    std::cout << "chunks:" << std::endl;
    for (const Chunk& c : chunks) {
        if (c.id == CHUNK_ID_NULL) continue;
        std::cout << "  - id: "       << c.getID()       << std::endl;
        std::cout << "    group: "    << c.getGroupID()  << std::endl;
        std::cout << "    count: "    << c.getCount()    << std::endl;
        std::cout << "    capacity: " << c.getCapacity() << std::endl;
    }
}

// =============================================================================
// ChunkManager Chunk Management
// =============================================================================

Chunk* ChunkManager::_newChunk(Archetype& a, GroupID g) {
    ChunkID id = CHUNK_ID_NULL;

    if (!chunkFreeIDs.empty()) {
        id = chunkFreeIDs.back();
        chunkFreeIDs.pop_back();
        chunks[id]._initialize(id, &a, g);
        return &chunks[id];
    }

    id = static_cast<ChunkID>(chunks.size());
    chunks.emplace_back();
    chunks.back()._initialize(id, &a, g);
    return &chunks[id];
}

void ChunkManager::_freeChunk(ChunkID id) {
    ASSERT(hasChunk(id), "ChunkID " << id << " does not exist.");
    chunks[id]._clear();
    chunkFreeIDs.push_back(id);
}

// =============================================================================
// ChunkManager ChunkList Management
// =============================================================================

ChunkList* ChunkManager::_getOrCreateChunkList(Archetype& a, GroupID g) {
    ChunkListKey key{a.getMask(), g};
    auto it = chunkLists.find(key);
    if (it == chunkLists.end()) {
        ChunkList newList{};
        return &chunkLists.emplace(key, std::move(newList)).first->second;
    }
    return &it->second;
}

void ChunkManager::_insertChunkIntoList(Chunk* chunk, ChunkList* list) {
    // add to chunk list
    chunk->nextChunk = nullptr;
    chunk->prevChunk = list->tailChunk;

    if (list->tailChunk) {
        list->tailChunk->nextChunk = chunk;
    } else {
        list->headChunk = chunk;
    }
    list->tailChunk = chunk;

    // add to open chunk list
    chunk->nextChunkOpen = nullptr;
    chunk->prevChunkOpen = list->tailChunkOpen;

    if (list->tailChunkOpen) {
        list->tailChunkOpen->nextChunkOpen = chunk;
    } else {
        list->headChunkOpen = chunk;
    }
    list->tailChunkOpen = chunk;
}

void ChunkManager::_removeChunkFromOpenList(Chunk* chunk, ChunkList* list) {
    if (chunk->prevChunkOpen) {
        chunk->prevChunkOpen->nextChunkOpen = chunk->nextChunkOpen;
    } else {
        list->headChunkOpen = chunk->nextChunkOpen;
    }

    if (chunk->nextChunkOpen) {
        chunk->nextChunkOpen->prevChunkOpen = chunk->prevChunkOpen;
    } else {
        list->tailChunkOpen = chunk->prevChunkOpen;
    }

    chunk->prevChunkOpen = nullptr;
    chunk->nextChunkOpen = nullptr;
}

// =============================================================================
// ChunkManager Entity Helper Functions
// =============================================================================



} // namespace ECS