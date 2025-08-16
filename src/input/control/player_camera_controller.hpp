#pragma once

#include "common/types.hpp"
#include "graphics/camera.hpp"

#include <glm/glm.hpp>

class PlayerCameraController {
public:
    void update(Camera& camera, const FrameState& frame);

private:
    bool mouseMiddlePrev = false;
    bool isTranslating = false;
};

void PlayerCameraController::update(Camera& camera, const FrameState& frame) {
    const WindowInput& window = frame.input.window;
    const MouseInput& mouse = frame.input.mouse;
    const KeyboardInput& keyboard = frame.input.keyboard;

    // handle window resize
    if (window.resized)
        camera.resize(glm::vec2(window.width, window.height));

    // dont move camera if user is selecting
    if (frame.states.isSelecting)
        return;

    // handle camera zoom
    if (mouse.wheel.moved) {
        camera.zoom(-mouse.wheel.y);
    }

    // handle camera movement
    bool keyL = keyboard.buttons[SDL_SCANCODE_LEFT];
    bool keyR = keyboard.buttons[SDL_SCANCODE_RIGHT];
    bool keyD = keyboard.buttons[SDL_SCANCODE_DOWN];
    bool keyU = keyboard.buttons[SDL_SCANCODE_UP];

    bool mouseAtEdgeL = mouse.x <= 0.0f;
    bool mouseAtEdgeR = mouse.x >= window.width - 1.0f;
    bool mouseAtEdgeD = mouse.y >= window.height - 1.0f;
    bool mouseAtEdgeU = mouse.y <= 0.0f;

    if (keyL || mouseAtEdgeL) camera.moveLeft(frame.dt);
    if (keyR || mouseAtEdgeR) camera.moveRight(frame.dt);
    if (keyD || mouseAtEdgeD) camera.moveDown(frame.dt);
    if (keyU || mouseAtEdgeU) camera.moveUp(frame.dt);

    // handle camera translation using middle mouse
    bool mouseMiddle = (mouse.buttons & SDL_BUTTON_MMASK) != 0;

    if (mouseMiddle && !mouseMiddlePrev)
        isTranslating = true;

    if (isTranslating && mouse.moved)
        camera.translate(glm::vec3(mouse.xrel, -mouse.yrel, 0.0f));

    if (!mouseMiddle && mouseMiddlePrev)
        isTranslating = false;

    mouseMiddlePrev = mouseMiddle;
}