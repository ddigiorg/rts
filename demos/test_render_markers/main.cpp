#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "graphics/render/markers_renderer.hpp"

#include <string>

using namespace Core;
// using namespace Graphics;

struct Transform {
    float x, y, w, h;
};

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup game objects
    MarkersRenderer markersRenderer;
    markersRenderer.resize(4);

    // marker
    Transform transform0{0.0f, 0.0f, 32.0f, 32.0f};
    markersRenderer.appendSubset(1, &transform0);

    // setup loop variables
    bool quit = false;
    FrameState frame;

    // loop
    while (!quit) {

        // handle inputs
        frame.input = inputManager.processEvents();

        // handle updates
        uiManager.update(frame);

        // handle renders
        sdlManager.clearWindow();
        markersRenderer.render(uiManager.getPlayerCamera());
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}