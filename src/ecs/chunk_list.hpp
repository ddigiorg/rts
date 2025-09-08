#pragma once

#include "ecs/chunk.hpp"

namespace ECS {

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

class ChunkList {
public:
    ChunkList(ChunkListKey key);

    void insertChunk(Chunk* chunk);
    void removeChunk(Chunk* chunk);
    void insertChunkOpen(Chunk* chunk);
    void removeChunkOpen(Chunk* chunk);

    Chunk* getNextOpenChunk() { return headChunkOpen; };

private:
    ChunkListKey key;
    uint32_t count;

    Chunk* headChunk;     // list node to head chunk
	Chunk* tailChunk;     // list node to tail chunk
	Chunk* headChunkOpen; // list node to head chunk with open entity slots
	Chunk* tailChunkOpen; // list node to tail chunk with open entity slots
};

ChunkList::ChunkList(ChunkListKey key) {
    this->key = key;
    this->count = 0;
    this->headChunk = nullptr;
    this->tailChunk = nullptr;
    this->headChunkOpen = nullptr;
    this->tailChunkOpen = nullptr;
}

void ChunkList::insertChunk(Chunk* chunk) {
    chunk->nextChunk = nullptr;
    chunk->prevChunk = tailChunk;

    if (tailChunk) {
        tailChunk->nextChunk = chunk;
    } else {
        headChunk = chunk;
    }

    tailChunk = chunk;

    count++;
}

void ChunkList::removeChunk(Chunk* chunk) {
    if (chunk->prevChunk) {
        chunk->prevChunk->nextChunk = chunk->nextChunk;
    } else {
        headChunk = chunk->nextChunk;
    }

    if (chunk->nextChunk) {
        chunk->nextChunk->prevChunk = chunk->prevChunk;
    } else {
        tailChunk = chunk->prevChunk;
    }

    chunk->prevChunk = nullptr;
    chunk->nextChunk = nullptr;

    count--;
}

void ChunkList::insertChunkOpen(Chunk* chunk) {
    chunk->nextChunkOpen = nullptr;
    chunk->prevChunkOpen = tailChunkOpen;

    if (tailChunkOpen) {
        tailChunkOpen->nextChunkOpen = chunk;
    } else {
        headChunkOpen = chunk;
    }

    tailChunkOpen = chunk;
}

void ChunkList::removeChunkOpen(Chunk* chunk) {
    if (chunk->prevChunkOpen) {
        chunk->prevChunkOpen->nextChunkOpen = chunk->nextChunkOpen;
    } else {
        headChunkOpen = chunk->nextChunkOpen;
    }

    if (chunk->nextChunkOpen) {
        chunk->nextChunkOpen->prevChunkOpen = chunk->prevChunkOpen;
    } else {
        tailChunkOpen = chunk->prevChunkOpen;
    }

    chunk->prevChunkOpen = nullptr;
    chunk->nextChunkOpen = nullptr;
}

} // namespace ECS