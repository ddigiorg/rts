// Movement.hpp
#pragma once

// #include "ecs/Types.hpp"
// #include "ecs/Archetype.hpp"
#include "ecs/Manager.hpp"

class Movement : public ECS::System {
public:
    void run() override {
        std::vector<ECS::Archetype*> archetypes = ecs->query({ECS::typeof(Position), ECS::typeof(Velocity)});

        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                auto pos = archetype->getComponentDataArray<Position>(c);
                auto vel = archetype->getComponentDataArray<Velocity>(c);

                for (size_t i = 0; i < archetype->getChunkNumEntities(c); i++) {
                    // EntityID = archetype->getEntityID(c, i);
                    pos[i].x += vel[i].x;
                    pos[i].y += vel[i].y;
                }
            }
        }

        // std::cout << "movement" << std::endl;
    }
};