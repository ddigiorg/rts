#pragma once

#include "ecs/manager.hpp"
#include "graphics/render/quads_renderer.hpp"

#include "core/components/global.hpp"
#include "core/components/transform.hpp"
#include "core/components/color.hpp"

using namespace ECS;

class QuadsUpdate : public System {
public:
    void run() override {
        std::vector<Archetype*> archetypes;

        // reset quad renderer
        auto global = ecs->getSingletonData<Global>();
        Graphics::QuadsRenderer* qr = global->quadsRenderer;
        qr->resetCount();

        // update quad renderer opengl buffers
        archetypes = ecs->query({typeof(Transform), typeof(Color)});
        for (Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t count = archetype->getChunkNumEntities(c);
                auto transforms = archetype->getComponentDataArray<Transform>(c);
                auto colors = archetype->getComponentDataArray<Color>(c);

                qr->appendSubset(count, transforms, colors);
            }
        }
    }
};