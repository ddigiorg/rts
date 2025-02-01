#pragma once

#include "ecs/manager.hpp"
#include "game/components/position.hpp"
#include "game/components/velocity.hpp"
#include "game/components/size.hpp"
#include "game/components/color.hpp"
#include "utilities/random.hpp"

class SpriteInitialize : public ECS::System {
public:
    SpriteInitialize::SpriteInitialize()
        : positionRNG(-100.0f, 100.0f),
          velocityRNG(-1.0f, 1.0f),
          sizeRNG(5.0f, 50.0f),
          colorRNG(0.0f, 1.0f)
    {};

    void run() override {
        std::vector<ECS::Archetype*> archetypes;

        // setup positions
        archetypes = ecs->query({ECS::typeof(Position)});
        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto pos = archetype->getComponentDataArray<Position>(c);
                for (size_t i = 0; i < numEntities; i++) {
                    pos[i].x = positionRNG.get();
                    pos[i].y = positionRNG.get();
                }
            }
        }

        // setup velocities
        archetypes = ecs->query({ECS::typeof(Velocity)});
        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto vel = archetype->getComponentDataArray<Velocity>(c);
                for (size_t i = 0; i < numEntities; i++) {
                    vel[i].x = velocityRNG.get();
                    vel[i].y = velocityRNG.get();
                }
            }
        }

        // setup sizes
        archetypes = ecs->query({ECS::typeof(Size)});
        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto size = archetype->getComponentDataArray<Size>(c);
                for (size_t i = 0; i < numEntities; i++) {
                    size[i].size = (float)(int)sizeRNG.get();
                }
            }
        }

        // setup colors
        archetypes = ecs->query({ECS::typeof(Color)});
        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto color = archetype->getComponentDataArray<Color>(c);
                for (size_t i = 0; i < numEntities; i++) {
                    color[i].r = colorRNG.get();
                    color[i].g = colorRNG.get();
                    color[i].b = colorRNG.get();
                }
            }
        }

    }

private:
    RandomFloatGenerator positionRNG;
    RandomFloatGenerator velocityRNG;
    RandomFloatGenerator sizeRNG;
    RandomFloatGenerator colorRNG;
};