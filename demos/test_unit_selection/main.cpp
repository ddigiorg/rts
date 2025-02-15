#include "core/frame_state.hpp"
#include "core/sdl_manager.hpp"
#include "input/input_manager.hpp"
#include "ui/ui_manager.hpp"
#include "graphics/render/sprites_renderer.hpp"
#include "utils/math.hpp"

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
    SpriteTransform transform0{-75.0f, 0.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord0{0.0f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform0, &texcoord0);

    // red sprite
    SpriteTransform transform1{-25.0f, 0.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord1{0.25f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform1, &texcoord1);

    // green sprite
    SpriteTransform transform2{25.0f, 0.0f, 32.0f, 64.0f};
    SpriteTexCoord texcoord2{0.5f, 0.0f, 0.25f, 1.0f};
    spritesRenderer.appendSubset(1, &transform2, &texcoord2);

    // blue sprite
    SpriteTransform transform3{75.0f, 0.0f, 32.0f, 64.0f};
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

        SelectEvent& select = frame.events.select;
        if (select.isActive) {
            const WindowInput& window = frame.input.window;
            const Camera& playerCamera = uiManager.getPlayerCamera();
            glm::vec2 begWorldPos = playerCamera.screenToWorld(select.boxBegX, select.boxBegY, window.width, window.height);
            glm::vec2 endWorldPos = playerCamera.screenToWorld(select.boxEndX, select.boxEndY, window.width, window.height);
            glm::vec2 boxSize = glm::abs(endWorldPos - begWorldPos);

            // std::cout << "(" << begWorldPos.x << ", " << begWorldPos.y << ")" << std::endl;
            // std::cout << "(" << endWorldPos.x << ", " << endWorldPos.y << ")" << std::endl;
            // std::cout << "(" << boxSize.x << ", " << boxSize.y << ")" << std::endl;
            // std::cout << std::endl;

            // TODO: not working right just yet...
            bool test = aabbIntersects(
                begWorldPos,
                boxSize,
                glm::vec2(transform0.x, transform0.y),
                glm::vec2(transform0.w, transform0.h)
            );

            std::cout << test << std::endl;

            select.isActive = false;
        }

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