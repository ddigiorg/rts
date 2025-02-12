#include "engine/core/sdl_manager.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/ui/cursor.hpp"
#include "engine/ui/debug_overlay.hpp"

#include "game/gfx/quads_renderer.hpp"
#include "game/input/player_camera_controller.hpp"
#include "game/input/screen_camera_controller.hpp"
#include "game/input/debug_overlay_controller.hpp"

#include <string>

using namespace Core;
using namespace GFX;
using namespace UI;

inline void _setupDebugOverlayLines(std::vector<DebugOverlayLine>& lines) {
    lines = {
            DebugOverlayLine("Press ESC to quit.", {5.0f, -20.0f}),
            DebugOverlayLine("Press arrow keys to move camera.", {5.0f, -40.0f}),
            DebugOverlayLine("Move mouse to screen edge to move camera.", {5.0f, -60.0f}),
            DebugOverlayLine("Press middle mouse button and move mouse to move camera.", {5.0f, -80.0f}),
            DebugOverlayLine("Camera.XY(World): ", {5.0f, -100.0f}),
        };
}

inline void _updateDebugOverlayLines(
        const Camera& camera,
        const UserInput& input,
        std::vector<DebugOverlayLine>& lines
) {
        glm::vec3 cameraPos = camera.getPosition();
        lines[4].text = "Camera.XY(World): (" + std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.y) + ")";
}

int main() {

    // setup sdl
    SDLManager sdl;

    // setup cameras
    Camera playerCamera(Camera::Type::Orthographic, Camera::Mode::Centered);
    Camera screenCamera(Camera::Type::Orthographic, Camera::Mode::ViewPort);

    // setup input controllers
    PlayerCameraController playerCameraController;
    ScreenCameraController screenCameraController;
    DebugOverlayController debugOverlayController;

    // setup user interface (ui)
    DebugOverlay debugOverlay(true);
    std::vector<DebugOverlayLine> lines;
    _setupDebugOverlayLines(lines);

    // setup game objects
    QuadsRenderer quadsRenderer;

    struct Quad {
        float transform[4] = {0.0f, 0.0f, 128.0f, 128.0f};
        float color[3] = {1.0f, 1.0f, 1.0f};
    } quad;

    quadsRenderer.resize(1);
    quadsRenderer.appendSubset(1, &quad.transform[0], &quad.color[0]);

    // setup loop variables
    bool quit = false;
    float dt = 1.0f;

    // loop
    while (!quit) {

        // handle inputs
        UserInput input = sdl.processEvents();
        playerCameraController.handleInputs(playerCamera, input, dt);
        screenCameraController.handleInputs(screenCamera, input);
        debugOverlayController.handleInputs(debugOverlay, input);

        // handle updates
        _updateDebugOverlayLines(playerCamera, input, lines);

        // handle renders
        sdl.clear();
        debugOverlay.render(screenCamera, lines);
        quadsRenderer.render(playerCamera);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}