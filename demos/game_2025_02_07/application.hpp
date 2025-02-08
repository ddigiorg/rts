#pragma once

#include "engine/core/user_input.hpp"
#include "engine/core/sdl_manager.hpp"
#include "engine/ecs/manager.hpp"
#include "engine/gfx/types.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/cursor.hpp"
#include "engine/gfx/debug_screen.hpp"
#include "engine/gfx/quad_renderer.hpp"
#include "engine/utilities/timer.hpp"

#include "game/ecs/components/global.hpp"
#include "game/ecs/components/position.hpp"
#include "game/ecs/components/velocity.hpp"
#include "game/ecs/components/size.hpp"
#include "game/ecs/components/color.hpp"
#include "game/ecs/systems/sprite_initialize.hpp"
#include "game/ecs/systems/sprite_update.hpp"
#include "game/ecs/systems/movement.hpp"
#include "game/ecs/events/events.hpp"
#include "game/ui/control_camera_player.hpp"
#include "game/ui/control_camera_screen.hpp"
#include "game/ui/control_debug_screen.hpp"


#include <iostream>

// TODO: change errors in entire codebase to throw std::runtime_error("Blah.");

class Application {
public:
    Application();
    ~Application();
    void loop();

private:
    Core::SDLManager sdl;

    GFX::Camera cameraPlayer;
    GFX::Camera cameraScreen;
    GFX::Cursor cursor;
    GFX::DebugScreen debugScreen;

    ControlCameraPlayer ctrlCameraPlayer;
    ControlCameraScreen ctrlCameraScreen;
    ControlDebugScreen ctrlDebugScreen;

    GFX::QuadRenderer quadRenderer;

    ECS::Manager ecs;

    Timer timer;
};

Application::Application()
    : sdl(),
      cameraPlayer(GFX::Camera::Type::Orthographic, GFX::Camera::Mode::Centered),
      cameraScreen(GFX::Camera::Type::Orthographic, GFX::Camera::Mode::ViewPort),
      debugScreen(true),
      ctrlCameraPlayer(cameraPlayer),
      ctrlCameraScreen(),
      ctrlDebugScreen(),
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
    float dt = 1.0f;

    while (!quit) {
        timer.reset();

        // process events
        Core::UserInput input = sdl.processEvents();

        ctrlCameraPlayer.handleInputs(cameraPlayer, input, dt);
        ctrlCameraScreen.handleInputs(cameraScreen, input);
        ctrlDebugScreen.handleInputs(debugScreen, input);

        // update
        cursor.update(cameraPlayer, input);
        ecs.triggerEvent<OnUpdate>();

        // render
        sdl.clear();
        quadRenderer.render(cameraPlayer);
        cursor.render(cameraPlayer);
        // debugScreen.render(cameraScreen, lines);
        sdl.swap();

        // frameCount++;
        // std::cout << timer.elapsed() << std::endl;

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
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