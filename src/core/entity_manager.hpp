#pragma once

#include "config/entity_configs.hpp"
#include "core/types.hpp"
#include "core/entity_spatial_grid.hpp"

#include <cstdint>
#include <array>
#include <vector>

#include <iostream>

struct EntityRecord {
    size_t dataIndex;
};

class EntityManager {
public:
    EntityID spawnEntity(const EntityType type, const Vec2f& position);
    void removeEntity(const EntityID id);
    void update();
    void updateRenderData(QuadRenderer& qr);

private:
    EntityID _getNextID();

    EntityConfigManager configManager;
    SpatialGrid grid;

    std::vector<EntityID> recycledIDs;
    std::vector<EntityRecord> records; // Note, each index is an EntityId

    // packed data
    std::vector<EntityID> ids;
    std::vector<EntityType> types;
    std::vector<Vec2f> positions;
    std::vector<Vec2f> velocities;
    std::vector<Vec2f> targets;
};

EntityID EntityManager::spawnEntity(const EntityType type, const Vec2f& position) {
    // TODO: ensure type exists
    EntityID id = _getNextID();

    size_t i = ids.size();
    ids.push_back(id);
    types.push_back(type);
    positions.push_back(position);
    velocities.push_back({0.0f, 0.0f});
    targets.push_back({0.0f, 0.0f});
    records[id].dataIndex = i;

    grid.insertEntity(
        id,
        position,
        configManager.renderXYBox(type),
        configManager.renderColor(type)
    );

    return id;
}

void EntityManager::removeEntity(const EntityID id) {
    // TODO: ensure id < entityRecords.size()
    EntityID backId = ids.back();
    EntityRecord& currRecord = records[id];
    EntityRecord& backRecord = records[backId];

    // handle entity data
    size_t i = currRecord.dataIndex;
    swap_and_pop(ids, i);
    swap_and_pop(types, i);
    swap_and_pop(positions, i);
    swap_and_pop(velocities, i);
    swap_and_pop(targets, i);
    backRecord.dataIndex = i;
    currRecord.dataIndex = ENTITY_NULL_INDEX;

    grid.removeEntity(id);

    recycledIDs.push_back(id);
}

void EntityManager::update() {
    targets[0] = positions[0];
    targets[1] = positions[1];
    targets[2] = positions[2];
    velocities[3] = Vec2f(1.0f, 0.0f);
    targets[3] = Vec2f(1024.0f, 32.0f);

    for (int i = 0; i < ids.size(); i++) {
        const auto& id = ids[i];
        auto& position = positions[i];
        auto& velocity = velocities[i];
        const auto& target = targets[i];

        if (position.x == target.x)
            velocity.x = 0.0f;
        else
            velocity.x = 1.0f;
        if (position.y == target.y)
            velocity.y = 0.0f;
        else
            velocity.y = 1.0f;

        position.x += velocity.x; // * dt
        position.y += velocity.y; // * dt

        if (velocity.x > 0.0f || velocity.y > 0.0f) {
            grid.translateEntity(id, position);
        }
    }
}

void EntityManager::updateRenderData(QuadRenderer& qr) {
    grid.updateRenderData(qr);
}

EntityID EntityManager::_getNextID() {
    EntityID id = ENTITY_NULL_ID;
    if (!recycledIDs.empty()) {
        id = recycledIDs.back();
        recycledIDs.pop_back();
    } else {
        id = records.size();
        records.emplace_back();
    }
    return id;
}