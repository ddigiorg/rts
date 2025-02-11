#pragma once

#include "engine/core/types.hpp"
#include "engine/core/user_input.hpp"
#include "engine/gfx/camera.hpp"

#include <glm/glm.hpp>

class PlayerCameraController {
public:
    void handleInputs(GFX::Camera& camera, const Core::UserInput& input, const float dt) {

        // handle mouse move
        if (input.mouse.moved) {
            mousePos = {input.mouse.x, input.mouse.y};
        }

        // handle window resize
        if (input.window.resized) {
            cameraSize = glm::vec2(input.window.width, input.window.height);
            camera.resize(cameraSize);
        }

        // get user input events
        bool keyL = input.keyboard.buttons[SDL_SCANCODE_LEFT];
        bool keyR = input.keyboard.buttons[SDL_SCANCODE_RIGHT];
        bool keyD = input.keyboard.buttons[SDL_SCANCODE_DOWN];
        bool keyU = input.keyboard.buttons[SDL_SCANCODE_UP];

        bool mouseAtEdgeL = mousePos.x == 0.0f;
        bool mouseAtEdgeR = mousePos.x == cameraSize.x - 1.0f;
        bool mouseAtEdgeD = mousePos.y == cameraSize.y - 1.0f;
        bool mouseAtEdgeU = mousePos.y == 0.0f;

        bool mouseMiddle = (input.mouse.buttons & SDL_BUTTON_MMASK) != 0;
        bool mouseWheel = input.mouse.wheel.moved;

        // handle camera movement
        if (keyL || mouseAtEdgeL) camera.moveLeft(dt);
        if (keyR || mouseAtEdgeR) camera.moveRight(dt);
        if (keyD || mouseAtEdgeD) camera.moveDown(dt);
        if (keyU || mouseAtEdgeU) camera.moveUp(dt);

        // handle camera zoom
        if (mouseWheel) {
            camera.zoom(-input.mouse.wheel.y);
        }

        // handle camera translation
        if (mouseMiddle && !mouseMiddlePrev){
            isTranslating = true;
        }
        if (isTranslating && input.mouse.moved) {
            camera.translate(glm::vec3(input.mouse.xrel, -input.mouse.yrel, 0.0f));
        }
        if (!mouseMiddle && mouseMiddlePrev) {
            isTranslating = false;
        }

        mouseMiddlePrev = mouseMiddle;
    }

private:
    glm::vec2 mousePos = {0.0f, 0.0f};
    glm::vec2 cameraSize = {Core::DEFAULT_WINDOW_WIDTH, Core::DEFAULT_WINDOW_HEIGHT};
    bool mouseMiddlePrev = false;
    bool isTranslating = false;
};