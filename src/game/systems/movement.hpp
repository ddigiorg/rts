#pragma once

#include "ecs/manager.hpp"
#include "game/components/position.hpp"
#include "game/components/velocity.hpp"
#include "utilities/random.hpp"

class Movement : public ECS::System {
public:
    Movement() : rng(-5.0f, 5.0f) {};

    void run() override {
        std::vector<ECS::Archetype*> archetypes = ecs->query({ECS::typeof(Position), ECS::typeof(Velocity)});

        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto pos = archetype->getComponentDataArray<Position>(c);
                auto vel = archetype->getComponentDataArray<Velocity>(c);

                for (size_t i = 0; i < numEntities; i++) {
                    // EntityID = archetype->getEntityID(c, i);
                    pos[i].x += vel[i].x;
                    pos[i].y += vel[i].y;
                    // vel[i].x = rng.get();
                    // vel[i].y = rng.get();

                    if (pos[i].x <= -400.0f || pos[i].x >= 400.0f)
                        vel[i].x = -vel[i].x;
                    if (pos[i].y <= -300.0f || pos[i].y >= 300.0f)
                        vel[i].y = -vel[i].y;
                }
            }
        }
    }
private:
    RandomFloatGenerator rng;
};