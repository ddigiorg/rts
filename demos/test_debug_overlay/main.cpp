#include "engine/core/sdl_manager.hpp"
#include "engine/ui/debug_overlay.hpp"

#include "game/input/screen_camera_controller.hpp"
#include "game/input/debug_overlay_controller.hpp"

#include <string>
#include <iostream>

using namespace Core;
using namespace GFX;
using namespace UI;

int main() {

    // setup sdl
    SDLManager sdl;

    // setup cameras
    Camera screenCamera(Camera::Type::Orthographic, Camera::Mode::ViewPort);

    // setup input controllers
    ScreenCameraController screenCameraController;
    DebugOverlayController debugOverlayController;

    // setup user interface (ui)
    DebugOverlay debugOverlay(true);
    std::vector<DebugOverlayLine> lines = {
        DebugOverlayLine("TEST OVERLAY", {5.0f, -20.0f}),
        DebugOverlayLine("Mouse.XY(Screen): ", {5.0f, -40.0f}),
    };

    // setup loop variables
    bool quit = false;

    // loop
    while (!quit) {

        // handle inputs
        UserInput input = sdl.processEvents();
        screenCameraController.handleInputs(screenCamera, input);
        debugOverlayController.handleInputs(debugOverlay, input);

        // handle updates
        int mouseX = (int)input.mouse.x;
        int mouseY = (int)input.mouse.y;
        lines[1].text = "Mouse.XY(Screen): (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";

        // handle renders
        sdl.clear();
        debugOverlay.render(screenCamera, lines);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}