#pragma once

#include "engine/core/user_input.hpp"
#include "engine/ui/debug_overlay.hpp"

class DebugOverlayController {
public:
    void handleInputs(UI::DebugOverlay& debugOverlay, const Core::UserInput& input) {
        bool gravePressed = input.keyboard.buttons[SDL_SCANCODE_GRAVE];

        // handle grave pressed
        if (gravePressed && !gravePressedPrev) {
            debugOverlay.toggleVisible();
        }

        gravePressedPrev = gravePressed;
    };

private:
    bool gravePressedPrev = false;
};