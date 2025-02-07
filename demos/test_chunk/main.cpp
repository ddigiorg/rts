#include "engine/core/sdl_manager.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/cursor.hpp"
#include "engine/gfx/debug_screen.hpp"
#include "engine/gfx/chunk_renderer.hpp"

#include "game/ui/control_camera_player.hpp"
#include "game/ui/control_camera_screen.hpp"
#include "game/ui/control_debug_screen.hpp"

#include <string>

using namespace Core;
using namespace GFX;

int main() {

    SDLManager sdl;

    Camera cameraPlayer(Camera::Type::Orthographic, Camera::Mode::Centered);
    Camera cameraScreen(Camera::Type::Orthographic, Camera::Mode::ViewPort);
    Cursor cursor;
    DebugScreen debugScreen(true);

    ControlCameraPlayer ctrlCameraPlayer(cameraPlayer);
    ControlCameraScreen ctrlCameraScreen;
    ControlDebugScreen ctrlDebugScreen;

    DebugScreenLine line0 = {
        "Press ESC to quit.",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -20.0f}
    };

    DebugScreenLine line1 = {
        "Press arrow keys to move camera.",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -40.0f}
    };

    DebugScreenLine line2 = {
        "Press middle mouse button and move mouse to move camera.",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -60.0f}
    };

    DebugScreenLine line3 = {
        "Camera.XY(World): ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -80.0f}
    };

    DebugScreenLine line4 = {
        "Mouse.XY(Screen): ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -100.0f}
    };

    std::vector<DebugScreenLine> lines = {line0, line1, line2, line3, line4};

    ChunkRenderer chunkRenderer;

    std::vector<glm::vec2> positions;
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            positions.push_back(glm::vec2(x, y));
        }
    }
    chunkRenderer.update(positions.data());

    bool quit = false;
    float dt = 1.0f;

    while (!quit) {
        UserInput input = sdl.processEvents();

        ctrlCameraPlayer.handleInputs(cameraPlayer, input, dt);
        ctrlCameraScreen.handleInputs(cameraScreen, input);
        ctrlDebugScreen.handleInputs(debugScreen, input);

        glm::vec3 pos = cameraPlayer.getPosition();
        std::string strCamX = std::to_string(pos.x);
        std::string strCamY = std::to_string(pos.y);
        lines[3].text = "Camera.XY(World): (" + strCamX + ", " + strCamY + ")";

        if (input.mouse.moved) {
            std::string strMouX = std::to_string(input.mouse.x);
            std::string strMouY = std::to_string(input.mouse.y);
            lines[4].text = "Mouse.XY(Screen): (" + strMouX + ", " + strMouY + ")";
        }

        cursor.update(cameraPlayer, input);

        sdl.clear();
        chunkRenderer.render(cameraPlayer);
        cursor.render(cameraPlayer);
        debugScreen.render(cameraScreen, lines);
        sdl.swap();

        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}