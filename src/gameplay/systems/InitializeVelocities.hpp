// InitializeVelocities.hpp
#pragma once

#include "ecs/Manager.hpp"
#include "gameplay/components/Velocity.hpp"
#include "utilities/Random.hpp"

class InitializeVelocities : public ECS::System {
public:
    InitializeVelocities() : rng(-1.0f, 1.0f) {};

    void run() override {
        std::vector<ECS::Archetype*> archetypes = ecs->query({ECS::typeof(Velocity)});

        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto vel = archetype->getComponentDataArray<Velocity>(c);

                for (size_t i = 0; i < numEntities; i++) {
                    // EntityID = archetype->getEntityID(c, i);
                    vel[i].x = rng.get();
                    vel[i].y = rng.get();
                }
            }
        }
    }

private:
    RandomFloatGenerator rng;
};