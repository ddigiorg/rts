#pragma once

#include "common/config.hpp"
#include "common/types.hpp"
#include "utils/assert.hpp"

#include <array>

// TODO: could potentailly make this constexpr EntityConfigStore to build
//       configs fully at compile time instead of runtime

constexpr size_t to_index(EntityType type) {
    return static_cast<size_t>(type);
}

class EntityConfigStore {
public:
    EntityConfigStore();

    const EntityConfig& getConfig(EntityType type) const;

private:
    void _loadEntityConfig(const EntityConfig& config);

    std::array<EntityConfig, to_index(EntityType::SIZE)> configs{};
};

EntityConfigStore::EntityConfigStore() {
    _loadEntityConfig(ENTITY_RED);
    _loadEntityConfig(ENTITY_GREEN);
    _loadEntityConfig(ENTITY_BLUE);
    _loadEntityConfig(ENTITY_YELLOW);
}

void EntityConfigStore::_loadEntityConfig(const EntityConfig& config) {
    configs[to_index(config.type)] = config;
}

const EntityConfig& EntityConfigStore::getConfig(EntityType type) const {
    ASSERT(type < EntityType::SIZE, "EntityType does not exist.");
    return configs[to_index(type)];
}