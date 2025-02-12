// include engine objects
#include "engine/core/sdl_manager.hpp"
#include "engine/ecs/manager.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/utilities/timer.hpp"

// include game renderers
#include "game/gfx/quads_renderer.hpp"

// include game input controllers
#include "game/input/player_camera_controller.hpp"
#include "game/input/screen_camera_controller.hpp"

// include game ecs components
#include "game/ecs/components/global.hpp"
#include "game/ecs/components/transform.hpp"
#include "game/ecs/components/velocity.hpp"
#include "game/ecs/components/color.hpp"

// include game ecs systems
#include "game/ecs/systems/quad_initialize.hpp"
#include "game/ecs/systems/quad_update.hpp"
#include "game/ecs/systems/movement.hpp"

// include game ecs events
#include "game/ecs/events/events.hpp"

using namespace Core;
using namespace ECS;
using namespace GFX;

int main() {

    // setup sdl
    SDLManager sdl;

    // setup cameras
    Camera playerCamera(Camera::Type::Orthographic, Camera::Mode::Centered);
    Camera screenCamera(Camera::Type::Orthographic, Camera::Mode::ViewPort);

    // setup input controllers
    PlayerCameraController playerCameraController;
    ScreenCameraController screenCameraController;

    // setup utility objects
    Timer timer;
    QuadsRenderer quadsRenderer;
    quadsRenderer.resize(2048);

    timer.reset();

    // setup ecs
    Manager ecs; // TODO: rename to ECSManager or World or something else

    // setup ecs components
    ecs.registerComponent<Global>();
    ecs.registerComponent<Transform>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Color>();

    // setup ecs systems
    ecs.registerSystem<Movement>();
    ecs.registerSystem<QuadInitialize>();
    ecs.registerSystem<QuadUpdate>();

    // setup ecs events
    ecs.registerEvent<OnCreate>();
    ecs.registerEvent<OnDestroy>();
    ecs.registerEvent<OnUpdate>();
    ecs.registerEvent<OnRender>();

    // subscribe systems to events
    ecs.subscribeToEvent<OnCreate, QuadInitialize>();
    ecs.subscribeToEvent<OnUpdate, Movement>();
    ecs.subscribeToEvent<OnUpdate, QuadUpdate>();

    // setup global singleton
    ecs.createSingleton<Global>();
    auto global = ecs.getSingletonData<Global>();
    global->quadsRenderer = &quadsRenderer;

    // setup entities
    for (size_t i = 0; i < 1000; i++) {
        ecs.createEntity({typeof(Transform), typeof(Velocity), typeof(Color)});
    }

    // handle ecs creation systems
    ecs.triggerEvent<OnCreate>();

    std::cout << "ecs create time: " << timer.elapsed() << std::endl;
    // ecs.print();

    // setup loop variables
    bool quit = false;
    float dt = 1.0f;

    // loop
    while (!quit) {

        // handle inputs
        UserInput input = sdl.processEvents();
        playerCameraController.handleInputs(playerCamera, input, dt);
        screenCameraController.handleInputs(screenCamera, input);

        // handle updates
        timer.reset();
        ecs.triggerEvent<OnUpdate>();
        std::cout << "ecs loop time: " << timer.elapsed() << std::endl;

        // handle renders
        sdl.clear();
        quadsRenderer.render(playerCamera);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    // handle any ecs cleanup systems
    timer.reset();
    ecs.triggerEvent<OnDestroy>();
    std::cout << "ecs destroy time: " << timer.elapsed() << std::endl;

    return 0;
}