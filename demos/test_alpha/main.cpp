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

    unitManager.create(UnitConfig{UNIT_CYAN, tileGridToWorld(Location{0, -10}), Scale{64.0f, 64.0f}});

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            Location loc = Location{x - 16, y};
            UnitConfig config {UNIT_CYAN, tileGridToWorld(loc)};
            unitManager.create(config);
        }
    }

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            Location loc = Location{x + 16, y};
            UnitConfig config {UNIT_MAGENTA, tileGridToWorld(loc)};
            unitManager.create(config);
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