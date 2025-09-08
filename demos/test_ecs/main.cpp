#include "ecs/world.hpp"

struct Disabled {};

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

int main() {
    ECS::World ecs;

    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Disabled>();

    ecs.createEntity(Position{ 1.0f,  2.0f}, Velocity{ 3.0f,  4.0f});
    ecs.createEntity(Position{ 5.0f,  6.0f}, Velocity{ 7.0f,  8.0f});
    ecs.createEntity(Position{ 9.0f, 10.0f}, Velocity{11.0f, 12.0f});

    ecs.removeEntity(1);

    ecs.createEntity(Position{13.0f, 14.0f});



    ecs.print();

    ECS::Chunk chunk0 = ecs.getChunk(0);
    int count0 = chunk0.getCount();
    auto* eID0 = chunk0.getEntityIDs();
    auto* pos0 = chunk0.data<Position>();
    auto* vel0 = chunk0.data<Velocity>();
    for (int i = 0; i < count0; i++) {
        if (i == 1) {
            pos0[i].x = 69.0f;
        }
        std::cout << eID0[i] << std::endl;
        std::cout << pos0[i].x << ", " << pos0[i].y << std::endl;
        std::cout << vel0[i].x << ", " << vel0[i].y << std::endl;
        std::cout << std::endl;
    }

    ECS::Chunk chunk1 = ecs.getChunk(1);
    int count1 = chunk1.getCount();
    auto* eID1 = chunk1.getEntityIDs();
    auto* pos1 = chunk1.data<Position>();
    for (int i = 0; i < count1; i++) {
        std::cout << eID1[i] << std::endl;
        std::cout << pos1[i].x << ", " << pos1[i].y << std::endl;
        std::cout << std::endl;
    }

    return 0;
}