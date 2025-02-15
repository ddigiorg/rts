#pragma once

#include "ecs/manager.hpp"
#include "utils/random.hpp"

#include "core/components/transform.hpp"
#include "core/components/velocity.hpp"
#include "core/components/color.hpp"

using namespace ECS;

class QuadsInitialize : public System {
public:
    QuadsInitialize::QuadsInitialize()
        : positionRNG(-100.0f, 100.0f),
          velocityRNG(-1.0f, 1.0f),
          sizeRNG(5.0f, 50.0f),
          colorRNG(0.0f, 1.0f)
    {};

    void run() override {
        std::vector<Archetype*> archetypes;

        // setup transform data
        archetypes = ecs->query({typeof(Transform)});
        for (Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t count = archetype->getChunkNumEntities(c);
                auto transform = archetype->getComponentDataArray<Transform>(c);
                for (size_t i = 0; i < count; i++) {
                    float size = (float)(int)sizeRNG.get();
                    transform[i].x = positionRNG.get();
                    transform[i].y = positionRNG.get();
                    transform[i].w = size;
                    transform[i].h = size;
                }
            }
        }

        // setup velocity data
        archetypes = ecs->query({typeof(Velocity)});
        for (Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t count = archetype->getChunkNumEntities(c);
                auto vel = archetype->getComponentDataArray<Velocity>(c);
                for (size_t i = 0; i < count; i++) {
                    vel[i].x = velocityRNG.get();
                    vel[i].y = velocityRNG.get();
                }
            }
        }

        // setup color data
        archetypes = ecs->query({typeof(Color)});
        for (Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t count = archetype->getChunkNumEntities(c);
                auto color = archetype->getComponentDataArray<Color>(c);
                for (size_t i = 0; i < count; i++) {
                    color[i].r = colorRNG.get();
                    color[i].g = colorRNG.get();
                    color[i].b = colorRNG.get();
                }
            }
        }
    };

private:
    RandomFloatGenerator positionRNG;
    RandomFloatGenerator velocityRNG;
    RandomFloatGenerator sizeRNG;
    RandomFloatGenerator colorRNG;
};