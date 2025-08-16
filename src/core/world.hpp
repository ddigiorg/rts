#pragma once

#include "core/entity_config_store.hpp"
#include "core/entity_manager.hpp"

class World {
public:
private:
    EntityConfigStore entityConfigs;
    EntityManager entityManager;
    // ChunkManager chunkManager;

    // TODO: make a RenderSystem or RenderManager?
    // QuadRenderer quadRenderer;
};