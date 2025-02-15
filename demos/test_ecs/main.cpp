#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "graphics/render/quads_renderer.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "utils/timer.hpp"

// include game ecs components
#include "core/components/global.hpp"
#include "core/components/transform.hpp"
#include "core/components/velocity.hpp"
#include "core/components/color.hpp"

// include game ecs systems
#include "core/systems/movement.hpp"
#include "core/systems/quads_initialize.hpp"
#include "core/systems/quads_update.hpp"

// include game ecs events
#include "core/events/events.hpp"

using namespace Core;
using namespace ECS;
// using namespace Graphics;

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

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
    ecs.registerSystem<QuadsInitialize>();
    ecs.registerSystem<QuadsUpdate>();

    // setup ecs events
    ecs.registerEvent<OnCreate>();
    ecs.registerEvent<OnDestroy>();
    ecs.registerEvent<OnUpdate>();
    ecs.registerEvent<OnRender>();

    // subscribe systems to events
    ecs.subscribeToEvent<OnCreate, QuadsInitialize>();
    ecs.subscribeToEvent<OnUpdate, Movement>();
    ecs.subscribeToEvent<OnUpdate, QuadsUpdate>();

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
    FrameState frame;

    // loop
    while (!quit) {

        // handle inputs
        frame.input = inputManager.processEvents();

        // handle updates
        timer.reset();
        ecs.triggerEvent<OnUpdate>();
        std::cout << "ecs loop time: " << timer.elapsed() << std::endl;
        uiManager.update(frame);

        // handle renders
        sdlManager.clearWindow();
        quadsRenderer.render(uiManager.getPlayerCamera());
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    // handle any ecs cleanup systems
    timer.reset();
    ecs.triggerEvent<OnDestroy>();
    std::cout << "ecs destroy time: " << timer.elapsed() << std::endl;

    return 0;
}