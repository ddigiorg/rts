#pragma once

#include "core/frame_state.hpp"
#include "graphics/camera.hpp"

#include <glm/glm.hpp>

using namespace Core;
using namespace Graphics;

class ScreenCameraController {
public:
    void update(Camera& camera, const FrameState& frame);
};

void ScreenCameraController::update(Camera& camera, const FrameState& frame) {
    const Input::WindowInput& window = frame.input.window;

    if (window.resized)
        camera.resize(glm::vec2(window.width, window.height));
};