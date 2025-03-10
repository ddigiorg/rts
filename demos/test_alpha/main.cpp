#include "core/sdl_manager.hpp"
#include "core/frame_state.hpp"
#include "core/world_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"

#include "utils/timer.hpp"

using namespace Core;
using namespace Input;
// using namespace UI;

int main() {

    Timer timer;

    // setup managers
    SDLManager sdlManager;
    // WorldManager worldManager;
    InputManager inputManager;
    UIManager uiManager;

    uiManager.toggleDebugOverlay();

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
        // worldManager.update();
        uiManager.update(frame);

        // handle renders
        sdlManager.clearWindow();
        // worldManager.render(playerCamera);
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