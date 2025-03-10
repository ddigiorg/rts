#pragma once

#include "utils/timer.hpp"

#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
// #include <limits> // for std::numeric_limits

#include <iostream>

// constexpr const uint32_t NULL_INDEX = std::numeric_limits<uint32_t>::max();

// =============================================================================
// Types
// =============================================================================

struct Position2f {
    float x, y;

    Position2f(float x = 0.0f, float y = 0.0f)
        : x(x), y(y) {}
};

struct Position2i {
    int32_t x, y;

    Position2i(int32_t x = 0, int32_t y = 0)
        : x(x), y(y) {}
};

struct BoundingBox {
    Position2f min, max;

    BoundingBox(
            const Position2f& min = Position2f(-0.5f, -0.5f),
            const Position2f& max = Position2f( 0.5f,  0.5f))
        : min(min), max(max) {}
};

// =============================================================================
// Entity Config
// =============================================================================

enum class EntityType : uint8_t {
    None = 0,
    Entity1,
    Entity2,
};

struct EntityConfig {
    BoundingBox collisionBox;
};

// TODO: reevaluate this
// TODO: perhaps storing a SoA would be better?
static const std::array<EntityConfig, 3> ENTITY_CONFIGS {
    EntityConfig{{ {0.0f, 0.0f}, {0.0f, 0.0f} }}, // None
    EntityConfig{{ {-16.0f, -16.0f}, {16.0f, 16.0f} }}, // Entity1
    EntityConfig{{ {-16.0f, -16.0f}, {16.0f, 16.0f} }}, // Entity2
};

// =============================================================================
// Tile
// =============================================================================

constexpr uint32_t TILE_PIXELS_X = 32;
constexpr uint32_t TILE_PIXELS_Y = 32;

// =============================================================================
// Sector2
// =============================================================================

constexpr uint32_t SECTOR2_TILE_COUNT_X = 2;
constexpr uint32_t SECTOR2_TILE_COUNT_Y = 2;
constexpr uint32_t SECTOR2_TILE_PIXELS_X = SECTOR2_TILE_COUNT_X * TILE_PIXELS_X;
constexpr uint32_t SECTOR2_TILE_PIXELS_Y = SECTOR2_TILE_COUNT_Y * TILE_PIXELS_Y;

// TODO: FINISH THIS
inline Position2i sector2WorldToIndex(const Position2f& world){
    int indexX = std::floor(world.x / SECTOR2_TILE_PIXELS_X);
    int indexY = std::floor(world.y / SECTOR2_TILE_PIXELS_Y);
    return Position2i(indexX, indexY);
}

struct Sector2 {
    bool dirty = false;
    uint8_t count = 0;
    uint8_t capacity = 0;
    std::vector<EntityType> types;     // 1 byte each
    std::vector<Position2f> positions; // 8 bytes each
    std::vector<Position2f> minBounds; // 8 bytes each
    std::vector<Position2f> maxBounds; // 8 bytes each

    void grow() {
        capacity += 2;
        types.reserve(capacity);
        positions.reserve(capacity);
        minBounds.reserve(capacity);
        maxBounds.reserve(capacity);
    }

    void updateBounds() {
        if (!dirty) return;

        for (uint8_t i = 0; i < count; i++) {
            const uint8_t type = static_cast<size_t>(types[i]);
            const BoundingBox& cbox = ENTITY_CONFIGS[type].collisionBox;
            minBounds[i] = { positions[i].x + cbox.min.x, positions[i].y + cbox.min.y };
            maxBounds[i] = { positions[i].x + cbox.max.x, positions[i].y + cbox.max.y };
        }
    }
};

// =============================================================================
// Sector8
// =============================================================================

constexpr uint32_t SECTOR8_TILE_COUNT_X = 8;
constexpr uint32_t SECTOR8_TILE_COUNT_Y = 8;

struct Sector8 {

    // uint8_t count;
    // TODO: add tile data
    // TODO: add static entity data

    // uint16_t renderCount;
    // TODO: add entity render data
};

// =============================================================================
// Chunk
// =============================================================================

constexpr uint32_t CHUNK_TILE_COUNT_X = 32;
constexpr uint32_t CHUNK_TILE_COUNT_Y = 32;
constexpr uint32_t CHUNK_TILE_PIXELS_X = CHUNK_TILE_COUNT_X * TILE_PIXELS_X;
constexpr uint32_t CHUNK_TILE_PIXELS_Y = CHUNK_TILE_COUNT_Y * TILE_PIXELS_Y;

inline Position2f chunkGridToWorld(const Position2i& grid){
    float worldX = grid.x * CHUNK_TILE_PIXELS_X;
    float worldY = grid.y * CHUNK_TILE_PIXELS_Y;
    return Position2f(worldX, worldY);
}

inline Position2i chunkWorldToGrid(const Position2f& world){
    int indexX = std::floor(world.x / CHUNK_TILE_PIXELS_X);
    int indexY = std::floor(world.y / CHUNK_TILE_PIXELS_Y);
    return Position2i(indexX, indexY);
}

inline uint64_t chunkGridToHash(const Position2i& grid) {
    return (uint64_t(uint32_t(grid.y)) << 32) | uint32_t(grid.x);
}

class Chunk {
public:
    Chunk(const Position2i& pos);

    bool dirty = false;
    Position2i pos;
    std::array<uint64_t, 8> neighborHashes;

    std::array<std::array<Sector8, 4>, 4> sector8s;
    std::array<std::array<Sector2, 16>, 16> sector2s;
};

Chunk::Chunk(const Position2i& pos) : pos(pos){
    neighborHashes[0] = chunkGridToHash(Position2i(pos.x + 1, pos.y + 0));
    neighborHashes[1] = chunkGridToHash(Position2i(pos.x + 1, pos.y + 1));
    neighborHashes[2] = chunkGridToHash(Position2i(pos.x + 0, pos.y + 1));
    neighborHashes[3] = chunkGridToHash(Position2i(pos.x - 1, pos.y + 1));
    neighborHashes[4] = chunkGridToHash(Position2i(pos.x - 1, pos.y + 0));
    neighborHashes[5] = chunkGridToHash(Position2i(pos.x - 1, pos.y - 1));
    neighborHashes[6] = chunkGridToHash(Position2i(pos.x + 0, pos.y - 1));
    neighborHashes[7] = chunkGridToHash(Position2i(pos.x + 1, pos.y - 1));
}

// =============================================================================
// World
// =============================================================================

class World {
public:
    void addChunk(const Position2i& pos);
    void delChunk(const Position2i& pos);

    void addEntity(const Position2f& pos, EntityType type);

    void runCollision();
    void print();

    size_t getChunkCount() const { return chunks.size(); };

private:
    void _computeSector2Bounds(Chunk& chunk);
    void _runChunkCollision(Chunk& chunk);
    void _runSector2Collision(Sector2& a, Sector2& b);

    std::unordered_map<uint64_t, Chunk> chunks;

    Timer timer;
};

void World::addChunk(const Position2i& pos) {
    uint64_t hash = chunkGridToHash(pos);
    chunks.emplace(hash, Chunk(pos));
}

void World::delChunk(const Position2i& pos) {
    uint64_t hash = chunkGridToHash(pos);
    chunks.erase(hash);
}

void World::addEntity(const Position2f& pos, EntityType type) {

    // Get chunk from entity position
    Position2i chunkPos = chunkWorldToGrid(pos);
    uint64_t chunkHash = chunkGridToHash(chunkPos);
    auto [it, inserted] = chunks.emplace(chunkHash, Chunk(chunkPos));
    Chunk& chunk = it->second;

    // Compute local sector index inside the chunk
    Position2i sector2Pos = sector2WorldToIndex(pos);
    sector2Pos.x -= chunkPos.x * (CHUNK_TILE_COUNT_X / SECTOR2_TILE_COUNT_X);
    sector2Pos.y -= chunkPos.y * (CHUNK_TILE_COUNT_Y / SECTOR2_TILE_COUNT_Y);
    Sector2& sector = chunk.sector2s[sector2Pos.y][sector2Pos.x];

    // Reallocate space for sector vectors if needed
    if (sector.count >= sector.capacity)
        sector.grow();

    // get entity collision box info
    const BoundingBox& cbox = ENTITY_CONFIGS[static_cast<uint8_t>(type)].collisionBox;

    // add entity
    sector.positions.push_back(pos);
    sector.types.push_back(type);
    sector.minBounds.push_back({pos.x + cbox.min.x, pos.y + cbox.min.y});
    sector.maxBounds.push_back({pos.x + cbox.max.x, pos.y + cbox.max.y});
    sector.count++;
    sector.dirty = true;
    chunk.dirty = true;
}

void World::runCollision() {
    for (auto& [hash, chunk] : chunks) {
        // if (!chunk.dirty) return;
        _computeSector2Bounds(chunk);
        _runChunkCollision(chunk);
    }
};

void World::print() {
    Chunk& chunk = chunks.at(0);

    for (uint8_t y = 0; y < 16; y++) {
        auto& row = chunk.sector2s[y];
        
        for (uint8_t x = 0; x < 16; x++) {
            std::cout << (uint32_t)row[x].count << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void World::_computeSector2Bounds(Chunk& chunk) {
    for (uint8_t y = 0; y < 16; y++) {
        auto& row = chunk.sector2s[y];

        for (uint8_t x = 0; x < 16; x++) {
            row[x].updateBounds();
        }
    }
}

void World::_runChunkCollision(Chunk& chunk) {
    // TODO: could probably further optimize this but it looks pretty good so far

    // assign chunk neighbors if they exist
    //
    // 3 2 1
    // 4 C 0
    // 5 6 7

    std::array<Chunk*, 8> neighbors {nullptr};

    for (size_t i = 0; i < 8; i++) {
        auto it = chunks.find(chunk.neighborHashes[i]);
        if (it != chunks.end())
            neighbors[i] = &it->second;
    }

    // handle chunk inner sectors
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   X X X Y   - - - -
    // - - - -   X X X Y   - - - -
    // - - - -   X X X Y   - - - -
    // - - - -   Y Y Y Y   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    for (uint8_t y = 0; y < 15; y++) {
        auto& row = chunk.sector2s[y];
        auto& nextRow = chunk.sector2s[y+1];

        for (uint8_t x = 0; x < 15; x++) {
            Sector2& sectorC  = row[x];
            if (!sectorC.dirty) continue;

            Sector2& sectorR  = row[x+1];
            Sector2& sectorB  = nextRow[x];
            Sector2& sectorBR = nextRow[x+1];

            _runSector2Collision(sectorC, sectorC);
            if (sectorR.dirty)
                _runSector2Collision(sectorC, sectorR);
            if (sectorB.dirty)
                _runSector2Collision(sectorC, sectorB);
            if (sectorBR.dirty)
                _runSector2Collision(sectorC, sectorBR);
        }
    }

    // handle right border sectors
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   - - - X   Y - - -
    // - - - -   - - - X   Y - - -
    // - - - -   - - - X   Y - - -
    // - - - -   - - - X   Y - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[0]) {
        for (uint8_t y = 0; y < 15; y++) {
            Sector2& sectorC  = chunk.sector2s[y][15];
            if (!sectorC.dirty) continue;

            Sector2& sectorR  = neighbors[0]->sector2s[y][0];
            Sector2& sectorBR = neighbors[0]->sector2s[y+1][0];

            _runSector2Collision(sectorC, sectorC);
            if (sectorR.dirty)
                _runSector2Collision(sectorC, sectorR);
            if (sectorBR.dirty)
                _runSector2Collision(sectorC, sectorBR);
        }

        Sector2& sectorC = chunk.sector2s[15][15];

        if (sectorC.dirty) {
            Sector2& sectorR = neighbors[0]->sector2s[15][0];

            _runSector2Collision(sectorC, sectorC);
            if (sectorR.dirty)
                _runSector2Collision(sectorC, sectorR);
        }
    }

    // handle top right corner sector
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   Y - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   - - - X   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[1]) {
        Sector2& sectorC  = chunk.sector2s[0][15];
        Sector2& sectorTR = neighbors[1]->sector2s[15][0];

        if (sectorC.dirty && sectorTR.dirty)
            _runSector2Collision(sectorC, sectorTR);
    }

    // handle top border sectors
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   Y Y Y Y   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   X X X X   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[2]) {
        for (uint8_t x = 0; x < 15; x++) {
            Sector2& sectorC = chunk.sector2s[0][x];
            if (!sectorC.dirty) continue;

            Sector2& sectorT  = neighbors[2]->sector2s[15][x];
            Sector2& sectorTR = neighbors[2]->sector2s[15][x+1];

            if (sectorT.dirty)
                _runSector2Collision(sectorC, sectorT);
            if (sectorTR.dirty)
                _runSector2Collision(sectorC, sectorTR);
        }

        Sector2& sectorC = chunk.sector2s[0][15];
        Sector2& sectorT = neighbors[2]->sector2s[15][15];

        if (sectorC.dirty && sectorT.dirty)
            _runSector2Collision(sectorC, sectorT);
    }

    // handle top left corner sector
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - Y   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   X - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[3]) {
        Sector2& sectorC  = chunk.sector2s[0][0];
        Sector2& sectorTL = neighbors[3]->sector2s[15][15];

        if (sectorC.dirty && sectorTL.dirty)
            _runSector2Collision(sectorC, sectorTL);
    }

    // handle left border sectors
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - Y   X - - -   - - - -
    // - - - Y   X - - -   - - - -
    // - - - Y   X - - -   - - - -
    // - - - Y   X - - -   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[4]) {
        for (uint8_t y = 0; y < 15; y++) {
            Sector2& sectorC = chunk.sector2s[y][0];
            if (!sectorC.dirty) continue;

            Sector2& sectorL  = neighbors[4]->sector2s[y][15];
            Sector2& sectorBL = neighbors[4]->sector2s[y+1][15];

            if (sectorL.dirty)
                _runSector2Collision(sectorC, sectorL);
            if (sectorBL.dirty)
                _runSector2Collision(sectorC, sectorBL);
        }

        Sector2& sectorC = chunk.sector2s[15][0];
        Sector2& sectorL = neighbors[4]->sector2s[15][15];

        if (sectorC.dirty && sectorL.dirty)
            _runSector2Collision(sectorC, sectorL);
    }

    // handle bottom left corner sector
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   X - - -   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - Y   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[5]) {
        Sector2& sectorC  = chunk.sector2s[15][0];
        Sector2& sectorBL = neighbors[5]->sector2s[0][15];

        if (sectorC.dirty && sectorBL.dirty)
            _runSector2Collision(sectorC, sectorBL);
    }

    // handle bottom border sectors
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   X X X X   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   Y Y Y Y   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[6]) {
        for (uint8_t x = 0; x < 15; x++) {
            Sector2& sectorC  = chunk.sector2s[15][x];
            if (!sectorC.dirty) continue;

            Sector2& sectorB  = neighbors[6]->sector2s[0][x];
            Sector2& sectorBR = neighbors[6]->sector2s[0][x+1];

            _runSector2Collision(sectorC, sectorC);
            if (sectorB.dirty)
                _runSector2Collision(sectorC, sectorB);
            if (sectorBR.dirty)
                _runSector2Collision(sectorC, sectorBR);
        }

        Sector2& sectorC = chunk.sector2s[15][15];

        if (sectorC.dirty) {
            Sector2& sectorB = neighbors[6]->sector2s[0][15];

            _runSector2Collision(sectorC, sectorC);
            if (sectorB.dirty)
                _runSector2Collision(sectorC, sectorB);
        }
    }

    // handle bottom right corner sector
    //
    // Neib[3]   Neib[2]   Neib[1]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // Neib[4]   Chunk     Neib[0]
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - X   - - - -
    // Neib[5]   Neib[6]   Neib[7]
    // - - - -   - - - -   Y - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -
    // - - - -   - - - -   - - - -

    if (neighbors[7]) {
        Sector2& sectorC  = chunk.sector2s[15][15];
        Sector2& sectorBR = neighbors[7]->sector2s[0][0];

        if (sectorC.dirty && sectorBR.dirty)
            _runSector2Collision(sectorC, sectorBR);
    }
}

void World::_runSector2Collision(Sector2& aSec, Sector2& bSec) {

    // timer.reset();

    for (uint8_t a = 0; a < aSec.count; a++) {
        Position2f& aPos = aSec.positions[a];
        const Position2f& aMin = aSec.minBounds[a];
        const Position2f& aMax = aSec.maxBounds[a];

        for (uint8_t b = 0; b < bSec.count; b++) {
            Position2f& bPos = bSec.positions[b];
            const Position2f& bMin = bSec.minBounds[b];
            const Position2f& bMax = bSec.maxBounds[b];

            // AABB collision check
            if (aMax.x <= bMin.x || aMin.x >= bMax.x ||
                aMax.y <= bMin.y || aMin.y >= bMax.y)
                continue;

            // // Compute overlap on both axes
            // float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
            // float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);

            // // Resolve collision on the smaller penetration axis
            // if (overlapX < overlapY) {
            //     float push = overlapX * 0.5f; // Distribute movement equally
            //     if (aPos.x < bPos.x) {
            //         aPos.x -= push;
            //         bPos.x += push;
            //     } else {
            //         aPos.x += push;
            //         bPos.x -= push;
            //     }
            // } else {
            //     float push = overlapY * 0.5f;
            //     if (aPos.y < bPos.y) {
            //         aPos.y -= push;
            //         bPos.y += push;
            //     } else {
            //         aPos.y += push;
            //         bPos.y -= push;
            //     }
            // }


            
            // aSec.dirty = false;
            // bSec.dirty = false;

            // std::cout << "collision!" << std::endl;

        }
    }

    // std::cout << timer.elapsed() << std::endl;
}