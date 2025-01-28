// InitializePositions.hpp
#pragma once

#include "ecs/Manager.hpp"
#include "gameplay/components/Position.hpp"
#include "utilities/Random.hpp"

class InitializePositions : public ECS::System {
public:
    InitializePositions() : rng(-200.0f, 200.0f) {};

    void run() override {
        std::vector<ECS::Archetype*> archetypes = ecs->query({ECS::typeof(Position)});

        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto pos = archetype->getComponentDataArray<Position>(c);

                for (size_t i = 0; i < numEntities; i++) {
                    // EntityID = archetype->getEntityID(c, i);
                    pos[i].x = rng.get();
                    pos[i].y = rng.get();
                }
            }
        }
    }

private:
    RandomFloatGenerator rng;
};