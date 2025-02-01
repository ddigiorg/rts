#pragma once

#include "core/Types.hpp"
#include "core/SDLManager.hpp"
#include "ecs/Manager.hpp"
#include "game/components/Global.hpp"
#include "game/components/Position.hpp"
#include "game/components/Velocity.hpp"
#include "game/components/Size.hpp"
#include "game/components/Color.hpp"
#include "game/systems/SpriteInitialize.hpp"
#include "game/systems/SpriteUpdate.hpp"
#include "game/systems/Movement.hpp"
#include "game/events/Events.hpp"
#include "graphics/Types.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Cursor.hpp"
#include "graphics/DebugScreen.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "utilities/Timer.hpp"

#include <iostream>

// TODO: change errors in entire codebase to throw std::runtime_error("Blah.");

class Application {
public:
    Application();
    ~Application();
    void loop();

private:
    Core::SDLManager sdl;
    Core::EventState events;

    GFX::Camera camera;
    GFX::Cursor cursor;
    GFX::DebugScreen debugScreen;
    GFX::SpriteRenderer spriteRenderer;

    ECS::Manager ecs;

    Timer timer;
};

Application::Application()
    : sdl(),
      camera(),
      cursor(),
      spriteRenderer(2048)
{
    std::cout << "initializing..." << std::endl;

    timer.reset();

    // setup ecs components
    ecs.registerComponent<Global>();
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Size>();
    ecs.registerComponent<Color>();

    // setup ecs systems
    ecs.registerSystem<Movement>();
    ecs.registerSystem<SpriteInitialize>();
    ecs.registerSystem<SpriteUpdate>();

    // setup ecs events
    ecs.registerEvent<OnCreate>();
    ecs.registerEvent<OnDestroy>();
    ecs.registerEvent<OnUpdate>();
    ecs.registerEvent<OnRender>();

    // subscribe systems to events
    ecs.subscribeToEvent<OnCreate, SpriteInitialize>();
    ecs.subscribeToEvent<OnUpdate, Movement>();
    ecs.subscribeToEvent<OnUpdate, SpriteUpdate>(); // TODO: perhaps this shoulde be OnRenderPre

    // setup global singleton
    ecs.createSingleton<Global>();
    auto global = ecs.getSingletonData<Global>();
    global->camera = &camera;
    global->cursor = &cursor;
    global->spriteRenderer = &spriteRenderer;

    // setup entities
    for (size_t i = 0; i < 1000; i++) {
        ecs.createEntity({
            ECS::typeof(Position),
            ECS::typeof(Velocity),
            ECS::typeof(Size),
            ECS::typeof(Color)
        });
    }

    // ecs.print();

    ecs.triggerEvent<OnCreate>();

    std::cout << timer.elapsed() << std::endl;
}

Application::~Application() {
    ecs.triggerEvent<OnDestroy>();
}

void Application::loop() {
    std::cout << "looping..." << std::endl;

    size_t frameCount = 0;

    while (!events.quit) {
        timer.reset();

        // process events
        sdl.processEvents(events);

        if (events.mouse.moved) {
            glm::vec2 mousePosWorld = camera.screenToWorld(
                events.mouse.posScreenX,
                events.mouse.posScreenY,
                events.window.width,
                events.window.height
            );
            events.mouse.posWorldX = mousePosWorld.x;
            events.mouse.posWorldY = mousePosWorld.y;
        }

        camera.handleEvents(events);
        cursor.handleEvents(events);
        debugScreen.handleEvents(events);

        // update data   
        camera.update();

        glm::vec3 cameraPos = camera.getPosition();
        debugScreen.update(DebugData{
            0, // fps
            (int)cameraPos.x,
            (int)cameraPos.y,
            (int)events.mouse.posScreenX,
            (int)events.mouse.posScreenY,
            (int)events.mouse.posWorldX,
            (int)events.mouse.posWorldX,
        });

        ecs.triggerEvent<OnUpdate>();

        // render
        sdl.clear();
        spriteRenderer.render(camera);
        cursor.render(camera);
        debugScreen.render(camera);
        sdl.swap();

        // frameCount++;
        // std::cout << timer.elapsed() << std::endl;
    }

    // // https://gameprogrammingpatterns.com/game-loop.html
    // // https://gafferongames.com/post/fix_your_timestep/
    // // https://docs.unity3d.com/Manual/ExecutionOrder.html
    // // Note: game time is in milliseconds
    // uint64_t tSim = 0; // ms
    // uint64_t tPrevious = SDL_GetTicks64();
    // uint64_t tLag = 0;
    // const uint64_t MS_PER_UPDATE = 16;
    // while (!quit) {
    //     uint64_t tCurrent = SDL_GetTicks64(); 
    //     uint64_t tFrame = tCurrent - tPrevious;
    //     tPrevious = tCurrent;
    //     tLag = tFrame;

    //     // TODO: figure out why middle mouse moving breaks...
    //     // basically sometimes we update more than once per processInput
    //     _processInput();

    //     // game simulates at constant rate
    //     while (tLag >= MS_PER_UPDATE) {
    //         _update(tSim, MS_PER_UPDATE);
    //         tLag -= MS_PER_UPDATE;
    //         tSim += MS_PER_UPDATE;
    //     }
    //     _render(); 
    //     fps = (int)(1.0 / ((float)tFrame / 1000.0f));
    // }
}