#pragma once

#include "engine/core/user_input.hpp"
#include "engine/gfx/camera.hpp"

#include <glm/vec2.hpp>

#include <iostream>

class ControlCameraPlayer {
public:
    ControlCameraPlayer(GFX::Camera& camera);
    void handleInputs(GFX::Camera& camera, const Core::UserInput& input, const float dt);

private:
    glm::vec2 mousePos = {0.0f, 0.0f};
    glm::vec2 cameraSize;
    bool mouseMPrev = false;
    bool isTranslating = false;
};

ControlCameraPlayer::ControlCameraPlayer(GFX::Camera& camera) {
    cameraSize = camera.getSize();
}

void ControlCameraPlayer::handleInputs(
        GFX::Camera& camera,
        const Core::UserInput& input,
        const float dt
) {
    if (input.mouse.moved) {
        mousePos = {input.mouse.x, input.mouse.y};
    }

    if (input.window.resized) {
        cameraSize = {input.window.width, input.window.height};
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

    bool mouseM = input.mouse.buttons == SDL_BUTTON_MMASK;
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
    if (mouseM && !mouseMPrev)
        isTranslating = true;

    if (isTranslating && input.mouse.moved) 
        camera.translate(glm::vec3(input.mouse.xrel, -input.mouse.yrel, 0.0f));

    if (!mouseM && mouseMPrev)
        isTranslating = false;

    mouseMPrev = mouseM;
}