#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "graphics/render/sprites_renderer.hpp"

using namespace Core;
// using namespace Graphics;

struct SpriteTransform {
    float x, y, w, h;
};

struct SpriteTexCoord {
    float u, v, w, h;
};

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup game objects
    SpritesRenderer spritesRenderer;
    spritesRenderer.resize(4);

    // white sprite
    SpriteTransform transform0{24.0f, 24.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord0{0.0f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform0, &texcoord0);

    // red sprite
    SpriteTransform transform1{16.0f, 16.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord1{0.25f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform1, &texcoord1);

    // green sprite
    SpriteTransform transform2{8.0f, 8.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord2{0.5f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform2, &texcoord2);

    // blue sprite
    SpriteTransform transform3{0.0f, 0.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord3{0.75f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform3, &texcoord3);

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
        spritesRenderer.render(uiManager.getPlayerCamera());
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}