#pragma once

#include "core/types.hpp"
#include "graphics/camera.hpp"

#include <glm/glm.hpp>

class ScreenCameraController {
public:
    void update(Camera& camera, const FrameState& frame);
};

void ScreenCameraController::update(Camera& camera, const FrameState& frame) {
    const WindowInput& window = frame.input.window;

    if (window.resized)
        camera.resize(glm::vec2(window.width, window.height));
};