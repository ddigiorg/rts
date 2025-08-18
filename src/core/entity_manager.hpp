#pragma once

#include "common/configs.hpp"
#include "common/types.hpp"
#include "graphics/render/quad_renderer.hpp"
#include "utils/assert.hpp"

#include <array>
#include <vector>

constexpr size_t to_index(EntityType type) {
    return static_cast<size_t>(type);
}

class EntityManager {
public:
    EntityManager();

    EntityID createEntity(EntityType type, const Vec2<float>& pos);
    void removeEntity(EntityID id);
    void updateEntities(); // TODO: input ChunkManager here?
    void updateRenderData(QuadRenderer& qr);

    const EntityConfig& getConfig(EntityType type) const;

private:
    constexpr void _loadConfig(const EntityConfig& config);

    std::array<EntityConfig, to_index(EntityType::SIZE)> configs{};

    std::vector<EntityIdx> id2idx;
    std::vector<EntityID> freeIds;

    EntityIdx count{0};
    std::vector<EntityID>    ids{};
    std::vector<EntityType>  types{};
    std::vector<Vec2<float>> positions{};
    std::vector<Vec2<float>> velocities{};
    std::vector<Vec2<float>> targets{}; // TODO: make targetIds to enable entities to follow other entities
    std::vector<BoundingBox> renderXYBoxes{};
    std::vector<Vec4<float>> renderColors{};
};

EntityManager::EntityManager() {
    _loadConfig(ENTITY_RED);
    _loadConfig(ENTITY_GREEN);
    _loadConfig(ENTITY_BLUE);
    _loadConfig(ENTITY_YELLOW);
}

EntityID EntityManager::createEntity(EntityType type, const Vec2<float>& pos) {
    EntityID id = ENTITY_ID_NULL;
    EntityIdx idx = count;
    EntityConfig config = getConfig(type);

    if (!freeIds.empty()) {
        id = freeIds.back();
        freeIds.pop_back();
    } else {
        id = id2idx.size();
        id2idx.push_back(ENTITY_IDX_NULL);
    }

    ids.push_back(id);
    types.push_back(type);
    positions.push_back(pos);
    velocities.push_back({0.0f, 0.0f});
    targets.push_back({0.0f, 0.0f});
    renderXYBoxes.push_back(config.renderXYBox);
    renderColors.push_back(config.color);

    renderXYBoxes.back().moveTo(pos);

    // if (id == 3) velocities.back() = {1.0f, 0.0f};

    // TODO: update chunk with entity id and copy of render boxes and colors
    // chunkMgr->onEntityCreated(id, pos, renderXYBoxes.back(), renderColors.back());

    id2idx[id] = idx;
    count++;

    return id;
}

void EntityManager::removeEntity(EntityID id) {
    ASSERT(id < id2idx.size(), "EntityID does not exist.");

    EntityID lastId = ids.back();
    EntityIdx idx = id2idx[id];

    vector_swap_pop(ids,           idx);
    vector_swap_pop(types,         idx);
    vector_swap_pop(positions,     idx);
    vector_swap_pop(velocities,    idx);
    vector_swap_pop(targets,       idx);
    vector_swap_pop(renderXYBoxes, idx);
    vector_swap_pop(renderColors,  idx);

    // chunkMgr->onEntityDestroyed(id);

    id2idx[lastId] = idx;
    id2idx[id] = ENTITY_IDX_NULL;
    count--;

    freeIds.push_back(id);
}

void EntityManager::updateEntities() {

    // move entities
    for (EntityIdx i = 0; i < count; ++i) {
        auto& pos = positions[i];
        const auto& vel = velocities[i];

        // NOTE: this eventually will be accelerated with SIMD
        pos.x += vel.x; // * dt
        pos.y += vel.y; // * dt

        if(vel.x != 0.0f || vel.y != 0.0f) {
            renderXYBoxes[i].moveTo(pos);
            // chunkMgr->queueEntityMoved(ids[i], pos); // TODO: this is a very hot loop so this should probably queue ChunkManager to do the work later
        }
    }
}

void EntityManager::updateRenderData(QuadRenderer& qr) {
    size_t offset = qr.size();
    
    if(count > 0) {
        qr.update(offset, count, renderXYBoxes.data(), renderColors.data());
    }
}

const EntityConfig& EntityManager::getConfig(EntityType type) const {
    ASSERT(type < EntityType::SIZE, "EntityType does not exist.");
    return configs[to_index(type)];
}

constexpr void EntityManager::_loadConfig(const EntityConfig& config) {
    configs[to_index(config.type)] = config;
}