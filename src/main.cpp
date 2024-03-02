#include "ecs/world.hpp"

#include <iostream>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

void Move(ECS::Chunk* chunk) {
    auto* p = chunk->GetData<Position>();
    auto* v = chunk->GetData<Velocity>();

    for(size_t i = 0; i < chunk->GetCount(); i++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

int main(int argc, char* argv[]) {
    ECS::World world;

    // register components
    ECS::ID cPosition = world.RegisterComponent<Position>();
    ECS::ID cVelocity = world.RegisterComponent<Velocity>();

    // register systems
    ECS::ID sMove = world.RegisterSystem(Move, {cPosition, cVelocity});

    // register events
    ECS::ID evUpdate = world.RegisterEvent({sMove});

    // create entities
    ECS::ID eUnit = world.CreateEntity();
    world.Set(eUnit, Position{0.0f, 0.0f});
    world.Set(eUnit, Velocity{1.0f, 1.0f});

    // print starting conditions
    Position* p = world.Get<Position>(eUnit);
    Velocity* v = world.Get<Velocity>(eUnit);
    printf("pos=(%f, %f), vel=(%f, %f)\n", p->x, p->y, v->x, v->y);

    // update 3 time steps
    world.SendEvent(evUpdate);
    world.SendEvent(evUpdate);
    world.SendEvent(evUpdate);
    world.ProcessEvents();
    printf("pos=(%f, %f), vel=(%f, %f)\n", p->x, p->y, v->x, v->y);

    // remove velocity component
    world.Del<Velocity>(eUnit);
    p = world.Get<Position>(eUnit);
    v = nullptr;

    // update 3 time steps
    world.SendEvent(evUpdate);
    world.SendEvent(evUpdate);
    world.SendEvent(evUpdate);
    world.ProcessEvents();
    printf("pos=(%f, %f)\n", p->x, p->y);

    // re-add velocity component
    world.Set(eUnit, Velocity{-1.0f, -1.0f});
    p = world.Get<Position>(eUnit);
    v = world.Get<Velocity>(eUnit);

    // update 3 time steps
    world.SendEvent(evUpdate);
    world.SendEvent(evUpdate);
    world.SendEvent(evUpdate);
    world.ProcessEvents();
    printf("pos=(%f, %f), vel=(%f, %f)\n", p->x, p->y, v->x, v->y);

    std::cout << "success" << std::endl;
    return 0;
}