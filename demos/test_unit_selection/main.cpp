#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "core/world_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "graphics/render/sprites_renderer.hpp"
#include "graphics/render/markers_renderer.hpp"
#include "utils/math.hpp"

#include <glm/glm.hpp>

#include <vector>

using namespace Core;
// using namespace Graphics;

struct Transform {
    float x, y, w, h;
};

struct TexCoord {
    float u, v, w, h;
};

int main() {

    // setup managers
    SDLManager sdlManager;
    InputManager inputManager;
    UIManager uiManager;
    uiManager.toggleDebugOverlay();

    // setup game object managers
    WorldManager worldManager;

    // setup game objects
    SpritesRenderer spritesRenderer;
    spritesRenderer.resize(4);

    MarkersRenderer markersRenderer;
    markersRenderer.resize(4);

    std::vector<Transform> transforms;
    std::vector<TexCoord> texcoords;

    // setup red sprite
    transforms.push_back(Transform{-75.0f, 0.0f, 32.0f, 64.0f});
    texcoords.push_back(TexCoord{0.0f, 0.0f, 0.25f, 1.0f});
    spritesRenderer.appendSubset(1, &transforms[0], &texcoords[0]);

    // setup green sprite
    transforms.push_back(Transform{-25.0f, 0.0f, 32.0f, 64.0f});
    texcoords.push_back(TexCoord{0.25f, 0.0f, 0.25f, 1.0f});
    spritesRenderer.appendSubset(1, &transforms[1], &texcoords[1]);

    // setup blue sprite
    transforms.push_back(Transform{25.0f, 0.0f, 32.0f, 64.0f});
    texcoords.push_back(TexCoord{0.5f, 0.0f, 0.25f, 1.0f});
    spritesRenderer.appendSubset(1, &transforms[2], &texcoords[2]);

    // setup yellow sprite
    transforms.push_back(Transform{75.0f, 0.0f, 32.0f, 64.0f});
    texcoords.push_back(TexCoord{0.75f, 0.0f, 0.25f, 1.0f});
    spritesRenderer.appendSubset(1, &transforms[3], &texcoords[3]);

    // setup loop variables
    bool quit = false;
    FrameState frame;
    const Camera& playerCamera =  uiManager.getPlayerCamera();

    // loop
    while (!quit) {

        // handle inputs
        frame.input = inputManager.processEvents();

        // handle updates
        uiManager.update(frame);

        // ========================================
        // handle select event
        // ========================================
        SelectEvent& select = frame.events.select;
        if (select.isActive) {
            const WindowInput& window = frame.input.window;
            const Camera& playerCamera = uiManager.getPlayerCamera();

            // convert mouse coordinates to world coordinates
            glm::vec2 begWorldPos = playerCamera.screenToWorld(select.boxBegX, select.boxBegY, window.width, window.height);
            glm::vec2 endWorldPos = playerCamera.screenToWorld(select.boxEndX, select.boxEndY, window.width, window.height);

            // compute selection box
            glm::vec2 boxMin = glm::min(begWorldPos, endWorldPos);
            glm::vec2 boxMax = glm::max(begWorldPos, endWorldPos);
            glm::vec2 boxSize = boxMax - boxMin;

            markersRenderer.resetCount();

            // compute collisions
            for (const Transform& transform : transforms) {
                glm::vec2 spritePos = glm::vec2(transform.x, transform.y);
                glm::vec2 spriteSize = glm::vec2(transform.w, transform.h);

                bool isSelected = checkAABBCollision(boxMin, boxSize, spritePos, spriteSize);

                if (isSelected) {
                    markersRenderer.appendSubset(1, &transform);
                }

                // std::cout << isSelected << " ";
            }
            // std::cout << std::endl;

            select.isActive = false;
        }

        // handle renders
        sdlManager.clearWindow();
        worldManager.render(playerCamera);
        spritesRenderer.render(playerCamera);
        markersRenderer.render(playerCamera);
        uiManager.render();
        sdlManager.swapWindowBuffers();

        // handle quit
        quit = frame.input.quit;
        if(frame.input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}