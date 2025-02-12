#pragma once

#include "engine/ecs/manager.hpp"
#include "engine/utilities/random.hpp"

#include "game/ecs/components/transform.hpp"
#include "game/ecs/components/velocity.hpp"

using namespace ECS;

class Movement : public System {
public:
    Movement() : rng(-5.0f, 5.0f) {};

    void run() override {
        std::vector<ECS::Archetype*> archetypes;
        
        archetypes = ecs->query({typeof(Transform), typeof(Velocity)});
        for (Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t count = archetype->getChunkNumEntities(c);
                auto pos = archetype->getComponentDataArray<Transform>(c);
                auto vel = archetype->getComponentDataArray<Velocity>(c);

                for (size_t i = 0; i < count; i++) {
                    // EntityID = archetype->getEntityID(c, i);
                    pos[i].x += vel[i].x;
                    pos[i].y += vel[i].y;

                    if (pos[i].x <= -400.0f || pos[i].x >= 400.0f)
                        vel[i].x = -vel[i].x;
                    if (pos[i].y <= -300.0f || pos[i].y >= 300.0f)
                        vel[i].y = -vel[i].y;
                }
            }
        }
    };

private:
    RandomFloatGenerator rng;
};