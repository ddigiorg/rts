#include "ecs/world.hpp"

struct Disabled : ECS::IComponentData {};

struct Position : ECS::IComponentData {
    float x, y;
};

struct Velocity : ECS::IComponentData {
    float x, y;
};

struct Poop : ECS::IComponentData {
    float x, y;
};

int main() {
    ECS::World ecs;

    // register components
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerTag<Disabled>();

    ecs.createEntity({ECS::typeof(Position), ECS::typeof(Velocity)});

    ecs.print();

    return 0;
}