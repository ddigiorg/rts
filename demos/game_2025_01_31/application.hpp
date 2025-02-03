#pragma once

#include "core/types.hpp"
#include "core/sdl_manager.hpp"
#include "ecs/manager.hpp"
#include "game/components/global.hpp"
#include "game/components/position.hpp"
#include "game/components/velocity.hpp"
#include "game/components/size.hpp"
#include "game/components/color.hpp"
#include "game/systems/sprite_initialize.hpp"
#include "game/systems/sprite_update.hpp"
#include "game/systems/movement.hpp"
#include "game/events/events.hpp"
#include "graphics/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/cursor.hpp"
#include "graphics/debug_screen.hpp"
#include "graphics/quad_renderer.hpp"
#include "utilities/timer.hpp"

#include <iostream>

// TODO: change errors in entire codebase to throw std::runtime_error("Blah.");

class Application {
public:
    Application();
    ~Application();
    void loop();

private:
    Core::SDLManager sdl;

    GFX::Camera camera;
    GFX::Cursor cursor;
    GFX::DebugScreen debugScreen;
    GFX::QuadRenderer quadRenderer;

    ECS::Manager ecs;

    Timer timer;
};

Application::Application()
    : sdl(),
      camera(),
      cursor(),
      debugScreen(),
      quadRenderer(2048)
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
    global->quadRenderer = &quadRenderer;

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
    glm::vec2 mousePosWorld = {0.0f, 0.0f};

    bool quit = false;
    while (!quit) {
        timer.reset();

        // process events
        Core::FrameInput input = sdl.processEvents();
        quit = input.quit;

        if (input.mouse.moved) {
            mousePosWorld = camera.screenToWorld(
                input.mouse.x,
                input.mouse.y,
                input.window.width,
                input.window.height
            );
        }

        camera.handleEvents(input);
        cursor.handleEvents(input, mousePosWorld);
        // debugScreen.handleEvents(input);

        // // update data   
        // camera.update();

        // glm::vec3 cameraPos = camera.getPosition();
        // debugScreen.update(DebugData{
        //     0, // fps
        //     (int)cameraPos.x,
        //     (int)cameraPos.y,
        //     (int)input.mouse.x,
        //     (int)input.mouse.y,
        //     (int)mousePosWorld.x,
        //     (int)mousePosWorld.y,
        // });

        // ecs.triggerEvent<OnUpdate>();

        // // render
        // sdl.clear();
        // quadRenderer.render(camera);
        // cursor.render(camera);
        // debugScreen.render(camera);
        // sdl.swap();

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