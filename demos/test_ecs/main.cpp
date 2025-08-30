#include "ecs/world.hpp"

struct Disabled {};

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};

// struct Share : ECS::ISharedComponentData {
//     float x, y;
// };

int main() {
    ECS::World ecs;

    // register components
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Disabled>();
    // ecs.registerSharedComponent<Share>();

    ecs.createEntity(Position{0.0f, 0.0f}, Velocity{1.0f, 1.0f});

    ecs.print();

    return 0;
}