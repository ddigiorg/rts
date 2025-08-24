#include "core/ecs.hpp"

struct Disabled : ECS::Tag {};

struct Position : ECS::Component {
    float x, y;
};

struct Velocity : ECS::Component {
    float x, y;
};

struct Poop : ECS::Component {
    float x, y;
};

int main() {
    ECS::Manager ecs;

    // register components
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerTag<Disabled>();

    ecs.createEntity({ECS::cidof(Position), ECS::cidof(Velocity)});

    // world.createEntity({ECS::typeof(Position), ECS::typeof(Velocity), ECS::typeof(Poop)});
    // world.createEntity({ECS::cidof(Position), ECS::cidof(Velocity)});

    ecs.print();

    return 0;
}