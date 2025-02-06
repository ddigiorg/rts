#pragma once

#include "engine/core/user_input.hpp"
#include "engine/gfx/camera.hpp"

#include <glm/vec2.hpp>

class ControlCameraScreen {
public:
    void handleInputs(GFX::Camera& camera, const Core::UserInput& input) {

        // handle resize
        if (input.window.resized) {
            glm::vec2 size = {input.window.width, input.window.height};
            camera.resize(size);
        }
    };
};