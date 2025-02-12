#pragma once

#include "engine/ecs/manager.hpp"

#include "game/gfx/quads_renderer.hpp"
#include "game/ecs/components/global.hpp"
#include "game/ecs/components/transform.hpp"
#include "game/ecs/components/color.hpp"

using namespace ECS;

class QuadUpdate : public System {
public:
    void run() override {
        std::vector<ECS::Archetype*> archetypes;

        // reset quad renderer
        auto global = ecs->getSingletonData<Global>();
        GFX::QuadsRenderer* qr = global->quadsRenderer;
        qr->resetCount();

        // update quad renderer opengl buffers
        archetypes = ecs->query({typeof(Transform), typeof(Color)});
        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t count = archetype->getChunkNumEntities(c);
                auto transforms = archetype->getComponentDataArray<Transform>(c);
                auto colors = archetype->getComponentDataArray<Color>(c);

                qr->appendSubset(count, transforms, colors);
            }
        }
    }
};