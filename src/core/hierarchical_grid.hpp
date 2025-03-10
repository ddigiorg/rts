#pragma once

#include "utils/timer.hpp"

#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
// #include <limits> // for std::numeric_limits

#include <execution>

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
// Sector
// =============================================================================

constexpr uint32_t SECTOR_TILE_COUNT_X = 8;
constexpr uint32_t SECTOR_TILE_COUNT_Y = 8;
constexpr uint32_t SECTOR_TILE_PIXELS_X = SECTOR_TILE_COUNT_X * TILE_PIXELS_X;
constexpr uint32_t SECTOR_TILE_PIXELS_Y = SECTOR_TILE_COUNT_Y * TILE_PIXELS_Y;

inline Position2i sectorWorldToGrid(const Position2f& world){
    int32_t gridX = std::floor(world.x / SECTOR_TILE_PIXELS_X);
    int32_t gridY = std::floor(world.y / SECTOR_TILE_PIXELS_Y);
    return Position2i(gridX, gridY);
}

struct SectorHash {
    size_t operator()(const Position2i& pos) const {
        return (std::hash<int32_t>()(pos.x) ^ (std::hash<int>()(pos.y) << 1));
    }
};

struct SectorEqual {
    bool operator()(const Position2i& p1, const Position2i& p2) const {
        return p1.x == p2.x && p1.y == p2.y;
    }
};

struct EntityData {
    Position2f position;
    Position2f minBound;
    Position2f maxBound;
    EntityType type;
};

struct Sector {
    bool dirty = true;
    uint8_t count = 0;
    uint8_t capacity = 0;
    std::vector<EntityData> entities;
    // std::vector<EntityType> types;     // 1 byte each
    // std::vector<Position2f> positions; // 8 bytes each
    // std::vector<Position2f> minBounds; // 8 bytes each
    // std::vector<Position2f> maxBounds; // 8 bytes each

    void grow() {
        capacity += 2;
        entities.reserve(capacity);
        // types.reserve(capacity);
        // positions.reserve(capacity);
        // minBounds.reserve(capacity);
        // maxBounds.reserve(capacity);
    }

    void updateBounds() {
        for (uint8_t i = 0; i < count; i++) {
            const uint8_t type = static_cast<size_t>(entities[i].type);
            const BoundingBox& cbox = ENTITY_CONFIGS[type].collisionBox;
            entities[i].minBound = { entities[i].position.x + cbox.min.x, entities[i].position.y + cbox.min.y };
            entities[i].maxBound = { entities[i].position.x + cbox.max.x, entities[i].position.y + cbox.max.y };
        }
    }
};

// =============================================================================
// Chunk
// =============================================================================

// constexpr uint32_t CHUNK_TILE_COUNT_X = 32;
// constexpr uint32_t CHUNK_TILE_COUNT_Y = 32;
// constexpr uint32_t CHUNK_TILE_PIXELS_X = CHUNK_TILE_COUNT_X * TILE_PIXELS_X;
// constexpr uint32_t CHUNK_TILE_PIXELS_Y = CHUNK_TILE_COUNT_Y * TILE_PIXELS_Y;

// inline Position2f chunkGridToWorld(const Position2i& grid){
//     float worldX = grid.x * CHUNK_TILE_PIXELS_X;
//     float worldY = grid.y * CHUNK_TILE_PIXELS_Y;
//     return Position2f(worldX, worldY);
// }

// inline Position2i chunkWorldToGrid(const Position2f& world){
//     int indexX = std::floor(world.x / CHUNK_TILE_PIXELS_X);
//     int indexY = std::floor(world.y / CHUNK_TILE_PIXELS_Y);
//     return Position2i(indexX, indexY);
// }

// inline uint64_t chunkGridToHash(const Position2i& grid) {
//     return (uint64_t(uint32_t(grid.y)) << 32) | uint32_t(grid.x);
// }

// class Chunk {
// public:
//     Chunk(const Position2i& pos);

//     Position2i pos;
// };

// Chunk::Chunk(const Position2i& pos)
//     : pos(pos){}

// =============================================================================
// World
// =============================================================================

class World {
public:
    void addEntity(const Position2f& pos, EntityType type);

    void runCollision();
    void runCollisionParallel();

    size_t getSectorCount() const { return sectors.size(); };

private:
    void _collideSectors(Sector& aSector, Sector& bSector);

    std::unordered_map<Position2i, Sector, SectorHash, SectorEqual> sectors;
    // std::unordered_map<Position2i, size_t, SectorHash, SectorEqual> sectors;
    // std::array<Sector, 20000> sectorDatas;

    Timer timer;
};

void World::addEntity(const Position2f& pos, EntityType type) {
    Position2i sectorPos = sectorWorldToGrid(pos);

    // create sector if it does not already exist
    auto [it, inserted] = sectors.emplace(sectorPos, Sector());
    Sector& sector = it->second;

    // reallocate space for sector vectors if needed
    if (sector.count >= sector.capacity)
        sector.grow();

    // get entity collision box info
    const BoundingBox& cbox = ENTITY_CONFIGS[static_cast<uint8_t>(type)].collisionBox;

    // add entity to sector
    sector.entities.push_back(EntityData {
        pos,
        {pos.x + cbox.min.x, pos.y + cbox.min.y},
        {pos.x + cbox.max.x, pos.y + cbox.max.y},
        type,
    });

    sector.count++;
}

void World::runCollision() {
    // for (auto& [sectorPos, sector] : sectors) {
    //     if (sector.count == 0 || !sector.dirty) continue;
    //     sector.updateBounds();
    // }

    // for (int i = 0; i < 16358; i++) {
    for (auto& [sectorPos, sector] : sectors) { // TODO: looping over map is slow, use vector instead
        if (sector.count == 0 || !sector.dirty) continue;

        // check entities within same sector
        _collideSectors(sector, sector);

        // check entities against neighboring sectors (avoid duplicate checks)
        for (int dy = 0; dy <= 1; dy++) {
            for (int dx = (dy == 0 ? 1 : -1); dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                Position2i neighborSectorPos = {sectorPos.x + dx, sectorPos.y + dy};
                auto it = sectors.find(neighborSectorPos);
                if (it == sectors.end() || it->second.count == 0 || !it->second.dirty) continue;

                _collideSectors(sector, it->second);
            }
        }
    }
}

// void World::runCollisionParallel() {
//     std::vector<std::pair<Sector*, Sector*>> collisionPairs;

//     for (auto& [sectorPos, sector] : sectors) {
//         if (sector.count == 0) continue;
//         sector.updateBounds();
//     }

//     for (auto& [sectorPos, sector] : sectors) {
//         if (sector.count == 0) continue;

//         // check entities within same sector
//         collisionPairs.push_back({&sector, &sector});

//         // check entities against neighboring sectors (avoid duplicate checks)
//         for (int dy = 0; dy <= 1; dy++) {
//             for (int dx = (dy == 0 ? 1 : -1); dx <= 1; dx++) {
//                 if (dx == 0 && dy == 0) continue;
//                 Position2i neighborSectorPos = {sectorPos.x + dx, sectorPos.y + dy};
//                 auto it = sectors.find(neighborSectorPos);
//                 if (it == sectors.end() || it->second.count == 0) continue;

//                 collisionPairs.push_back({&sector, &it->second});
//             }
//         }
//     }

//     // Parallel collision checks
//     std::for_each(std::execution::par, collisionPairs.begin(), collisionPairs.end(),
//     [&](std::pair<Sector*, Sector*>& pair) {
//         _collideSectors(*pair.first, *pair.second);
//     });
// }

void World::_collideSectors(Sector& aSector, Sector& bSector) {
    for (EntityData& aEnt : aSector.entities) {
        for (EntityData& bEnt : bSector.entities) {

            // AABB collision check
            if (aEnt.maxBound.x <= bEnt.minBound.x || aEnt.minBound.x >= bEnt.maxBound.x ||
                aEnt.maxBound.y <= bEnt.minBound.y || aEnt.minBound.y >= bEnt.maxBound.y)
                continue;

            // std::cout << "collision!" << std::endl;
        }
    }
}