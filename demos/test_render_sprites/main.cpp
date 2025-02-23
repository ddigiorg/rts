#include "core/types.hpp"
#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "graphics/render/sprite_renderer.hpp"

#include <vector>

using namespace Core;
// using namespace Graphics;

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup sprite renderer
    size_t capacity = 4;
    size_t count = 4;
    SpriteRenderer spriteRenderer;
    spriteRenderer.setCapacity(capacity);
    spriteRenderer.setCount(count);

    std::vector<Position> positions(count);
    std::vector<Scale> scales(count);
    std::vector<TexCoord> texcoords(count);

    // sprite: terrain grass
    positions[0] = Position{0.0f, 0.0f, 0.0f};
    scales[0] = Scale{32.0f, 32.0f};
    texcoords[0] = SPRITE_TEXCOORDS[SPRITE_TERRAIN_GRASS];

    // sprite: terrain water
    positions[1] = Position{32.0f, 0.0f, 0.0f};
    scales[1] = Scale{32.0f, 32.0f};
    texcoords[1] = SPRITE_TEXCOORDS[SPRITE_TERRAIN_WATER];

    // sprite: unit cyan
    positions[2] = Position{0.0f, 0.0f, 0.0f};
    scales[2] = Scale{32.0f, 32.0f};
    texcoords[2] = SPRITE_TEXCOORDS[SPRITE_UNIT_CYAN_270];

    // sprite: unit magenta
    positions[3] = Position{32.0f, 0.0f, 0.0f};
    scales[3] = Scale{32.0f, 32.0f};
    texcoords[3] = SPRITE_TEXCOORDS[SPRITE_UNIT_MAGENTA_270];

    spriteRenderer.updatePositionData(0, count, &positions[0]);
    spriteRenderer.updateScaleData(0, count, &scales[0]);
    spriteRenderer.updateTexCoordData(0, count, &texcoords[0]);

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
        spriteRenderer.render(uiManager.getPlayerCamera());
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}