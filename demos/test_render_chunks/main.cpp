#include "core/sdl_manager.hpp"
#include "ui/ui_manager.hpp"
#include "input/input_manager.hpp"
#include "graphics/render/chunk_renderer.hpp"

using namespace Core;
// using namespace Graphics;
using namespace UI;

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // renderers
    ChunkRenderer chunkRenderer(4);

    std::vector<glm::vec2> chunkPositions = {
        ChunkToWorld({0, 0}),
        ChunkToWorld({1, 0}),
        ChunkToWorld({0, 1}),
        ChunkToWorld({1, 1}),
    };

    chunkRenderer.updateChunkPosition(0, &chunkPositions[0]);
    chunkRenderer.updateChunkPosition(1, &chunkPositions[1]);
    chunkRenderer.updateChunkPosition(2, &chunkPositions[2]);
    chunkRenderer.updateChunkPosition(3, &chunkPositions[3]);

    // chunkRenderer.updateChunkTiles(1, &chunkTiles[0]);
    
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
        chunkRenderer.render(uiManager.getPlayerCamera());
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}