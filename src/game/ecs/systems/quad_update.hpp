#pragma once

#include "engine/ecs/manager.hpp"
#include "engine/gfx/quad_renderer.hpp"

#include "game/ecs/components/global.hpp"
#include "game/ecs/components/position.hpp"
#include "game/ecs/components/size.hpp"
#include "game/ecs/components/color.hpp"

class QuadUpdate : public ECS::System {
public:
    void run() override {
        auto global = ecs->getSingletonData<Global>();
        GFX::QuadRenderer* qr = global->quadRenderer;

        qr->resetCount();

        std::vector<ECS::Archetype*> archetypes = ecs->query({
            ECS::typeof(Position),
            ECS::typeof(Size),
            ECS::typeof(Color)
        });

        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto positions = archetype->getComponentDataArray<Position>(c);
                auto sizes = archetype->getComponentDataArray<Size>(c);
                auto colors = archetype->getComponentDataArray<Color>(c);

                qr->appendSubset(numEntities, positions, sizes, colors);
            }
        }
    }
};