#include "core/sdl_manager.hpp"
#include "ui/ui_manager.hpp"
#include "input/input_manager.hpp"

#include "core/world_manager.hpp"
#include "core/unit_manager.hpp"

using namespace Core;
using namespace UI;

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup world manager
    WorldManager worldManager;
    UnitManager unitManager;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            UnitConfig config {
                CYAN, // type
                tileIndexToWorld(Index{x - 16, y}), // position
            };
            unitManager.create(config);
        }
    }


    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            UnitConfig config {
                MAGENTA, // type
                tileIndexToWorld(Index{x + 16, y}), // position
            };
            unitManager.create(config);
        }
    }

    // setup loop variables
    bool quit = false;
    FrameState frame;
    const Camera& playerCamera = uiManager.getPlayerCamera();

    // loop
    while (!quit) {

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
    }

    return 0;
}