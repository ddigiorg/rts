#pragma once

#include "core/types.hpp"

#include <array>

// =============================================================================
// Definitions
// =============================================================================

enum class EntityType {
    RED_QUAD,
    GREEN_QUAD,
    BLUE_QUAD,
    YELLOW_QUAD,
    SIZE,
};

constexpr size_t to_index(EntityType t) {
    return static_cast<size_t>(t);
}

struct EntityConfig {
    EntityType type;
    Vec4f renderColor;
    BoundingBox renderXYBox;  // sprite position in world space
    // BoundingBox collisionBox; // for entity physics
    // BoundingBox selectionBox; // for entity selection
};

// =============================================================================
// Entity Configs
// =============================================================================

const EntityConfig RED_QUAD_CONFIG {
    /*type:*/ EntityType::RED_QUAD,
    /*color:*/ {1.0f, 0.0f, 0.0f, 1.0f},
    /*renderXYBox:*/ {{-16.0f, -16.0f}, {16.0f, 16.0f}},
};

const EntityConfig GREEN_QUAD_CONFIG {
    /*type:*/ EntityType::GREEN_QUAD,
    /*color:*/ {0.0f, 1.0f, 0.0f, 1.0f},
    /*renderXYBox:*/ {{-16.0f, -16.0f}, {16.0f, 16.0f}},
};

const EntityConfig BLUE_QUAD_CONFIG {
    /*type:*/ EntityType::BLUE_QUAD,
    /*color:*/ {0.0f, 0.0f, 1.0f, 1.0f},
    /*renderXYBox:*/ {{-16.0f, -16.0f}, {16.0f, 16.0f}},
};

const EntityConfig YELLOW_QUAD_CONFIG {
    /*type:*/ EntityType::YELLOW_QUAD,
    /*color:*/ {1.0f, 1.0f, 0.0f, 1.0f},
    /*renderXYBox:*/ {{-16.0f, -16.0f}, {16.0f, 16.0f}},
};

// =============================================================================
// Entity Config Manager
// =============================================================================

class EntityConfigManager {
public:
    EntityConfigManager();

    const Vec4f& renderColor(EntityType t) const { return renderColors[to_index(t)]; }
    const BoundingBox& renderXYBox(EntityType t) const { return renderXYBoxes[to_index(t)]; }

private:
    void _loadEntityConfig(const EntityConfig& config);

    std::array<Vec4f,       to_index(EntityType::SIZE)> renderColors;
    std::array<BoundingBox, to_index(EntityType::SIZE)> renderXYBoxes;
};

EntityConfigManager::EntityConfigManager() {
    _loadEntityConfig(RED_QUAD_CONFIG);
    _loadEntityConfig(GREEN_QUAD_CONFIG);
    _loadEntityConfig(BLUE_QUAD_CONFIG);
    _loadEntityConfig(YELLOW_QUAD_CONFIG);
}

void EntityConfigManager::_loadEntityConfig(const EntityConfig& config) {
    auto idx = to_index(config.type);
    renderColors[idx] = config.renderColor;
    renderXYBoxes[idx] = config.renderXYBox;
}