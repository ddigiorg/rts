#pragma once

#include "common/types.hpp"
#include "ui/debug_overlay.hpp"

class DebugOverlayController {
public:
    void update(DebugOverlay& debugOverlay, const FrameState& frame);

private:
    bool gravePressedPrev = false;
};

void DebugOverlayController::update(DebugOverlay& debugOverlay, const FrameState& frame) {

    // toggle active
    bool gravePressed = frame.input.keyboard.buttons[SDL_SCANCODE_GRAVE];
    if (gravePressed && !gravePressedPrev) {
        debugOverlay.toggleActive();
    }
    gravePressedPrev = gravePressed;
};