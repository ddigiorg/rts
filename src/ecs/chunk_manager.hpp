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

struct ChunkListKey {
    ArchetypeMask archetype;
    GroupID group;

    bool operator==(const ChunkListKey& other) const {
        return archetype == other.archetype && group == other.group;
    }
};

// https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/
// https://github.com/matteo65/mzHash64

struct ChunkListHasher {
    size_t operator()(const ChunkListKey& key) const {
        size_t h1 = std::hash<ArchetypeMask>{}(key.archetype);
        size_t h2 = std::hash<GroupID>{}(key.group);

        // boost-style hash combine
        // return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));       
    }
};

struct ChunkList {
    Chunk* headChunkActive = nullptr;
	Chunk* tailChunkActive = nullptr;
	Chunk* headChunkOpen   = nullptr;
	Chunk* tailChunkOpen   = nullptr;
};

class ChunkManager {
public:
    template<typename... Components>
    void insertEntity(Entity& e, Archetype& a, GroupID g, Components&&... data);
    // void removeEntity(EntityID id);
    // void moveEntityToGroup()
    // void insertEntityComponent();
    // void removeEntityComponent();

    // bool hasChunk(ChunkID id) const;
    // const Chunk* getChunk(ChunkID id);
    
    // bool hasChunkList();
    // const ChunkList* getChunkList(ArchetypeMask a, GroupID g);

    // miscellaneous
    void print();

private:
    // chunk management
    // Chunk& _newChunk();
    // void   _freeChunk(ChunkID id);

    // chunk list management
    ChunkList& _getOrCreateChunkList(Archetype& a, GroupID g);
    // void insertChunkToList(Chunk* c, ArchetypeMask a, GroupID g);
    // void removeChunkFromList(Chunk* c, ArchetypeMask a, GroupID g);

    // chunk storage, empty chunks recycled for later reuse
    std::deque<Chunk> chunks;
	std::vector<ChunkID> freeIDs;

    // chunk list storage, organized by archetype and group
    std::unordered_map<ChunkListKey, ChunkList, ChunkListHasher> chunkLists;
};

template<typename... Components>
void ChunkManager::insertEntity(Entity& e, Archetype& a, GroupID g, Components&&... data) {
    std::cout << e.getID() << std::endl;
    std::cout << a.getMask() << std::endl;
    std::cout << g << std::endl;
    ChunkList& list = _getOrCreateChunkList(a, g);

    // TODO: STOPPED HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Chunk& chunk = list.getOpenChunk();
    // chunk.insertEntity(entity, std::forward<Components>(data)...);
}

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

ChunkList& ChunkManager::_getOrCreateChunkList(Archetype& a, GroupID g) {
    ChunkListKey key{a.getMask(), g};
    auto it = chunkLists.find(key);
    if (it == chunkLists.end()) {
        ChunkList newList{};
        return chunkLists.emplace(key, std::move(newList)).first->second;
    }
    return it->second;
}

// Chunk& ChunkManager::newChunk() {
//     ChunkID id = CHUNK_ID_NULL;

//     if (!freeIDs.empty()) {
//         id = freeIDs.back();
//         freeIDs.pop_back();
//         chunks[id].id = id;
//         return chunks[id];
//     }

//     id = static_cast<ChunkID>(chunks.size());
//     chunks.emplace_back();
//     chunks.back().id = id;
//     return chunks[id];
// }

// void ChunkManager::freeChunk(ChunkID id) {
//     ASSERT(hasChunk(id), "ChunkID " << id << " does not exist.");
//     chunks[id].reset();
//     freeIDs.push_back(id);
// }

// bool ChunkManager::hasChunk(ChunkID id) const {
//     if (id < static_cast<ChunkID>(chunks.size()))
//         return !(chunks[id].id == CHUNK_ID_NULL);
//     return false;
// }

// Chunk& ChunkManager::getChunk(ChunkID id) {
//     ASSERT(hasChunk(id), "ChunkID " << id << " does not exist.");
//     return chunks[id];
// }



} // namespace ECS