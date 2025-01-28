// Application.hpp
#pragma once

#include "core/Types.hpp"
#include "core/SDLManager.hpp"
#include "ecs/Manager.hpp"
#include "gameplay/components/Position.hpp"
#include "gameplay/components/Velocity.hpp"
#include "gameplay/systems/InitializePositions.hpp"
#include "gameplay/systems/InitializeVelocities.hpp"
#include "gameplay/systems/Movement.hpp"
#include "gameplay/events/Events.hpp"
#include "graphics/CameraManager.hpp"
#include "graphics/Shader.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "utilities/Timer.hpp"

#include <SDL.h>

#include <iostream>

constexpr char* SPRITE_VERT_FILEPATH = "data/shaders/sprite_vert.glsl";
constexpr char* SPRITE_FRAG_FILEPATH = "data/shaders/sprite_frag.glsl";

// TODO: change errors in entire codebase to throw std::runtime_error("Blah.");

class Application {
public:
    Application();
    ~Application();
    void loop();

private:
    Core::SDLManager sdl;

    GFX::CameraManager camera;
    GFX::Shader spriteShader;
    GFX::SpriteRenderer spriteRenderer;

    ECS::Manager ecs;

    Timer timer;
};

Application::Application()
    : sdl(),
      camera(),
      spriteShader(SPRITE_VERT_FILEPATH, SPRITE_FRAG_FILEPATH),
      spriteRenderer(2048)
{
    std::cout << "initializing..." << std::endl;

    timer.reset();

    // setup ecs components
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();

    // setup ecs systems
    ecs.registerSystem<Movement>();
    ecs.registerSystem<InitializePositions>();
    ecs.registerSystem<InitializeVelocities>();

    // setup ecs events
    ecs.registerEvent<OnCreate>();
    ecs.registerEvent<OnDestroy>();
    ecs.registerEvent<OnUpdate>();
    ecs.registerEvent<OnRender>();

    // subscribe systems to events
    ecs.subscribeToEvent<OnCreate, InitializePositions>();
    ecs.subscribeToEvent<OnCreate, InitializeVelocities>();
    ecs.subscribeToEvent<OnUpdate, Movement>();

    // setup entities
    for (size_t i = 0; i < 256; i++)
        ecs.createEntity({ECS::typeof(Position), ECS::typeof(Velocity)});

    // ecs.print();

    ecs.triggerEvent<OnCreate>();

    std::cout << timer.elapsed() << std::endl;
}

Application::~Application() {
    ecs.triggerEvent<OnDestroy>();
}

void Application::loop() {
    std::cout << "looping..." << std::endl;

    bool quit = false;
    Core::GameEvents gameEvents;

    while (!gameEvents.quit) {
        timer.reset();

        // process events
        gameEvents = sdl.processEvents();

        // update data
        camera.update();
        ecs.triggerEvent<OnUpdate>();

        // TODO: put this somewhere... should ECS own it or SpriteRenderer? Leaning towards SpriteRenderer...
        std::vector<ECS::Archetype*> archetypes = ecs.query({ECS::typeof(Position)});
        for (ECS::Archetype* archetype : archetypes) {
            for (size_t c = 0; c < archetype->getNumChunks(); c++) {
                size_t numEntities = archetype->getChunkNumEntities(c);
                auto pos = archetype->getComponentDataArray<Position>(c);
                spriteRenderer.append(numEntities, pos);
            }
        }

        // render
        sdl.clear();
        spriteRenderer.render(spriteShader, camera);
        sdl.swap();

        std::cout << timer.elapsed() << std::endl;
    }
}