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
    SpriteRenderer spriteRenderer;
    spriteRenderer.setCapacity(100000);
    WorldManager worldManager;
    UnitManager unitManager;

    worldManager.update(spriteRenderer);
    size_t offset = spriteRenderer.getCount();

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            UnitConfig config {
                UNIT_CYAN, // type
                tileIndexToWorld(Index{x - 16, y}), // position
            };
            unitManager.create(config);
        }
    }

    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            UnitConfig config {
                UNIT_MAGENTA, // type
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

        timer.reset();

        // handle inputs
        frame.input = inputManager.processEvents();

        // handle updates
        spriteRenderer.setCount(offset); // TODO: handle this better
        uiManager.update(frame);
        unitManager.update(frame, playerCamera, spriteRenderer);

        // handle renders
        sdlManager.clearWindow();
        spriteRenderer.render(playerCamera);
        // unitManager.render(playerCamera);
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;

        std::cout << timer.elapsed() << std::endl;
    }

    return 0;
}