#include "core/sdl_manager.hpp"
#include "ui/ui_manager.hpp"
#include "input/input_manager.hpp"

#include "core/world_manager.hpp"
#include "core/unit_manager.hpp"
#include "graphics/render/sprite_renderer.hpp"

#include "utils/timer.hpp"

using namespace Core;
using namespace UI;

int main() {

    Timer timer;

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup world manager
    WorldManager worldManager;
    UnitManager unitManager;

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            Location loc = Location{x - 16, y};
            Position pos = tileGridToWorld(loc);
            unitManager.create(UnitType::BALL_CYAN, pos);
        }
    }

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            Location loc = Location{x + 16, y};
            Position pos = tileGridToWorld(loc);
            unitManager.create(UnitType::BALL_MAGENTA, pos);
        }
    }


    // setup loop variables
    bool quit = false;
    FrameState frame;
    const Camera& playerCamera = uiManager.getPlayerCamera();

    // loop
    while (!quit) {

        timer.reset();

        // handle inputs
        frame.input = inputManager.processEvents();

        // handle updates
        uiManager.update(frame);
        unitManager.update(frame, playerCamera);

        // handle renders
        sdlManager.clearWindow();
        worldManager.render(playerCamera);
        unitManager.render(playerCamera);
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;

        // std::cout << timer.elapsed() << std::endl;
    }

    return 0;
}