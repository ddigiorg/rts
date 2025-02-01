#pragma once

#include "ecs/manager.hpp"
#include "game/components/global.hpp"
#include "game/components/position.hpp"
#include "game/components/size.hpp"
#include "game/components/color.hpp"
#include "graphics/sprite_renderer.hpp"

class SpriteUpdate : public ECS::System {
public:
    void run() override {
        auto global = ecs->getSingletonData<Global>();
        GFX::SpriteRenderer* sr = global->spriteRenderer;

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

                sr->append(numEntities, positions, sizes, colors);
            }
        }
    }
};