#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "graphics/render/quads_renderer.hpp"

using namespace Core;
using namespace Graphics;
using namespace Input;
using namespace UI;

struct StaticQuad {
    const float transform[4] = {0.0f, 0.0f, 128.0f, 128.0f};
    const float color[3] = {0.5f, 0.5f, 0.5f};
};

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup renderers
    StaticQuad quad;
    QuadsRenderer quadsRenderer;
    quadsRenderer.resize(1);
    quadsRenderer.appendSubset(1, &quad.transform[0], &quad.color[0]);

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
        quadsRenderer.render(uiManager.getPlayerCamera());
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}