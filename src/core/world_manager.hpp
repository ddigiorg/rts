#pragma once

#include "core/types.hpp"
#include "core/components/chunk_data.hpp"
#include "core/components/color4f.hpp"
#include "core/components/position2i.hpp"
#include "core/components/position3f.hpp"
#include "core/components/scale2f.hpp"
#include "core/components/tags.hpp"
#include "core/components/texcoord4f.hpp"
#include "core/components/tile_data.hpp"
#include "core/components/velocity2f.hpp"
#include "core/events/events.hpp"
#include "core/systems/chunk_init.hpp"

#include "ecs/manager.hpp"

#include "graphics/camera.hpp"
#include "graphics/render/sprite_renderer.hpp"
#include "graphics/render/quad_renderer.hpp"

#include <vector>

using namespace Graphics;

namespace Core {

class WorldManager {
public:
    WorldManager();
    void update(); // include delta time
    void render(const Camera& camera);

private:
    void _setupComponents();
    void _setupSystems();
    void _setupEvents();
    void _setupEntities();

    ECS::Manager ecs;

    // TODO: If your rendering layer frequently reads from your vectors, consider 
    // using persistent mapped buffers or double-buffering for GPU updates. This
    // will prevent unnecessary CPU-GPU sync stalls.
    // renderers
    SpriteRenderer spriteRenderer;
    // QuadRenderer tGridRenderer;
    // QuadRenderer cGridRenderer;

};

WorldManager::WorldManager() {
    _setupComponents();
    _setupSystems();
    _setupEvents();
    _setupEntities();

    ecs.triggerEvent<OnCreate>();


    ecs.addEntityComponent<Active>(1);

    spriteRenderer.setCapacity(CHUNK_TILE_COUNT);
    spriteRenderer.setCount(CHUNK_TILE_COUNT);

    std::vector<Archetype*> archetypes = ecs.query({
        typeof(Active),
        typeof(Position3f),
        typeof(Scale2f),
        typeof(TexCoord4f),
    });

    size_t offset = 0;
    for (Archetype* archetype : archetypes) {
        for (size_t c = 0; c < archetype->getNumChunks(); c++) {
            size_t count = archetype->getChunkNumEntities(c);

            auto pos = archetype->getComponentDataArray<Position3f>(c);
            auto sca = archetype->getComponentDataArray<Scale2f>(c);
            auto tex = archetype->getComponentDataArray<TexCoord4f>(c);

            spriteRenderer.updatePositionData(offset, count, pos);
            spriteRenderer.updateScaleData(offset, count, sca);
            spriteRenderer.updateTexCoordData(offset, count, tex);

            offset += count;
        }
    }

}

void WorldManager::update() {
    // _updateRenderers();
}

void WorldManager::render(const Camera& camera) {
    spriteRenderer.render(camera);
    // tGridRenderer.renderOutline(camera, tLineWidth);
    // cGridRenderer.renderOutline(camera, cLineWidth);
}

void WorldManager::_setupComponents() {
    ecs.registerComponent<ChunkData>();
    ecs.registerComponent<Color4f>();
    ecs.registerComponent<Position2i>();
    ecs.registerComponent<Position3f>();
    ecs.registerComponent<Scale2f>();
    ecs.registerComponent<TexCoord4f>();
    ecs.registerComponent<TileData>();
    ecs.registerComponent<Velocity2f>();

    ecs.registerTag<Active>();
    ecs.registerTag<Visible>();
}

void WorldManager::_setupSystems() {
    ecs.registerSystem<ChunkInit>();
}

void WorldManager::_setupEvents() {
    ecs.registerEvent<OnCreate>();
    ecs.registerEvent<OnDestroy>();
    ecs.registerEvent<OnUpdate>();
    ecs.registerEvent<OnRender>();

    ecs.subscribeToEvent<OnCreate, ChunkInit>();
}

void WorldManager::_setupEntities() {

    // create chunk entities
    size_t cCount = 1;

    for(size_t i = 0; i < cCount; i++) {
        ecs.createEntity({
            typeof(ChunkData),
            typeof(Position2i),
        });
    }

    // create tile entities
    size_t tCount = cCount * CHUNK_TILE_COUNT;

    for(size_t i = 0; i < tCount; i++) {
        ecs.createEntity({
            typeof(TileData),
            typeof(Position3f),
            typeof(Scale2f),
            typeof(TexCoord4f),
        });
    }
}

} // namespace Core