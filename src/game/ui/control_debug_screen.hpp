#pragma once

#include "engine/core/user_input.hpp"
#include "engine/gfx/debug_screen.hpp"

#include <iostream>

class ControlDebugScreen {
public:
    void handleInputs(GFX::DebugScreen& debug, const Core::UserInput& input) {
        bool gravePressed = input.keyboard.buttons[SDL_SCANCODE_GRAVE];

        // handle grave pressed
        if (gravePressed && !gravePressedPrev)
            debug.toggleVisible();

        gravePressedPrev = gravePressed;
    };

private:
    bool gravePressedPrev = false;
};