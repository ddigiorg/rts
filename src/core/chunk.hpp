#pragma once

#include "common/config.hpp"
#include "common/functions.hpp"
#include "common/types.hpp"

#include <array>
#include <vector>

// dx={-1, 0, 1}, dy={-1, 0, 1}
constexpr const int CHUNK_NEIGHBOR_LUT[3][3] = {
    { 4,  0,  6},
    { 2, -1,  3},
    { 5,  1,  7}
};

class Chunk {
public:
    Chunk(const Vec2i& position);

    void createEntity(EntityId id, const EntityType& type, const Vec2f& position);
    void removeEntity(EntityIdx idx);
    void moveEntities();

    void updateRenderData();

private:
    void moveEntityTo(Chunk& dst, EntityIdx idx);

    Vec2i gridPosition;

    // border data
    BoundingBox border;
    // std::array<Chunk*, 8> neighbors;  // 0=l, 1=r, 2=d, 3=u, 4=dl, 5=dr, 6=ul, 7=ur
    // std::array<std::vector<EntityIdx>, 8> neighborTransfers; // TODO: could make these fixed-capacity arrays for no heap usage at all

    // entity data
    std::vector<EntityId> eIds;
    std::vector<EntityType> eTypes;
    std::vector<Vec2f> ePositions;
    std::vector<Vec2f> eVelocities;
    std::vector<Vec2f> eTargets;
    std::vector<BoundingBox> eRenderXYBoxes;
    // std::vector<BoundingBox> eRenderUVBoxes;
    std::vector<Vec4f> eRenderColors;
};

Chunk::Chunk(const Vec2i& position) {
    gridPosition = position;

    float minX = position.x * CHUNK_PIXELS_X;
    float minY = position.y * CHUNK_PIXELS_Y;
    float maxX = minX + CHUNK_PIXELS_X;
    float maxY = minY + CHUNK_PIXELS_Y;
    border = {minX, minY, maxX, maxY};
}

void Chunk::createEntity(EntityId id, const EntityType& type, const Vec2f& position) {
    eIds.push_back(id);
    eTypes.push_back(type);
    ePositions.push_back(position);
    eVelocities.push_back({0.0f, 0.0f});
    eTargets.push_back({0.0f, 0.0f});
    // ...
}

void Chunk::removeEntity(EntityIdx idx) {
    vectorRemove(eIds,           idx);
    vectorRemove(eTypes,         idx);
    vectorRemove(ePositions,     idx);
    vectorRemove(eVelocities,    idx);
    vectorRemove(eTargets,       idx);
    vectorRemove(eRenderXYBoxes, idx);
    vectorRemove(eRenderColors,  idx);
}

void Chunk::moveEntityTo(Chunk& dst, EntityIdx idx) {
    vectorMoveTo(dst.eIds,           eIds,           idx);
    vectorMoveTo(dst.eTypes,         eTypes,         idx);
    vectorMoveTo(dst.ePositions,     ePositions,     idx);
    vectorMoveTo(dst.eVelocities,    eVelocities,    idx);
    vectorMoveTo(dst.eTargets,       eTargets,       idx);
    vectorMoveTo(dst.eRenderXYBoxes, eRenderXYBoxes, idx);
    vectorMoveTo(dst.eRenderColors,  eRenderColors,  idx);
}

void Chunk::moveEntities() {
    for (size_t i = 0; i < ePositions.size(); ++i) {
        auto&      pos = ePositions[i];
        const auto vel = eVelocities[i];

        // move entity
        pos.x += vel.y;
        pos.y += vel.y;

        eRenderXYBoxes[i].translate(pos);

        // // handle border crossings
        // int dx = int(pos.x >= border.max.x) - int(pos.x < border.min.x);
        // int dy = int(pos.y >= border.max.y) - int(pos.y < border.min.y);
        // int neighborIndex = CHUNK_NEIGHBOR_LUT[dx + 1][dy + 1];
        // size_t isCrossing = size_t(neighborIndex >= 0);
        // neighborTransfers[neighborIndex & -(int)isCrossing].push_back(i);
    }
}

// void Chunk::flushTransferBuffers() {
//     for (int n = 0; n < 8; ++n) {
//         auto& buf = transferBuffers[n];
//         if (buf.empty()) continue;

//         Chunk* dst = neighbors[n];
//         if (!dst) {
//             buf.clear(); // TODO: drop or handle off-map entities
//             continue;
//         }

//         for (size_t idx: buf) {
//             moveEnntityTo(*dst, idx);
//         }
//         buf.clear();
//     }
// }