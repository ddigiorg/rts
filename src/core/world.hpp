#pragma once

#include "core/chunk_manager.hpp"
#include "core/entity_manager.hpp"

class World {
public:
private:
    ChunkManager chunkManager;
    EntityManager entityManager;

    // TODO: make a RenderSystem or RenderManager?
    // QuadRenderer quadRenderer;
};