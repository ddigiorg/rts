#include "engine/core/sdl_manager.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/chunk_renderer.hpp"
#include "engine/ui/cursor.hpp"
#include "engine/ui/debug_overlay.hpp"

#include "game/input/player_camera_controller.hpp"
#include "game/input/screen_camera_controller.hpp"
#include "game/input/debug_overlay_controller.hpp"

#include <string>

using namespace Core;
using namespace GFX;
using namespace UI;

int main() {

    // setup sdl
    SDLManager sdl;

    // setup cameras
    Camera cameraPlayer(Camera::Type::Orthographic, Camera::Mode::Centered);
    Camera cameraScreen(Camera::Type::Orthographic, Camera::Mode::ViewPort);

    // setup input controllers
    PlayerCameraController playerCameraController;
    ScreenCameraController screenCameraController;
    DebugOverlayController debugOverlayController;

    // setup user interface (ui)
    Cursor cursor;
    DebugOverlay debugOverlay(true);

    // setup debug information
    std::vector<DebugOverlayLine> lines = {
        DebugOverlayLine("Camera.XY(World): ", {5.0f, -20.0f}),
        DebugOverlayLine("Mouse.XY(Screen): ", {5.0f, -40.0f}),
        DebugOverlayLine("Mouse.XY(World): ", {5.0f, -60.0f}),
        DebugOverlayLine("Chunk.XY: ", {5.0f, -80.0f}),
        DebugOverlayLine("Tile.XY: ", {5.0f, -100.0f}),
    };

    // setup chunk information
    std::vector<glm::vec2> chunkOffsets = {
        ChunkToWorld({0, 0}),
        ChunkToWorld({1, 0}),
        ChunkToWorld({0, 1}),
        ChunkToWorld({1, 1}),
    };

    std::vector<GLubyte> chunkTiles = {
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    ChunkRenderer chunkRenderer(4);
    chunkRenderer.updateSubset(0, &chunkOffsets[0], nullptr);
    chunkRenderer.updateSubset(1, &chunkOffsets[1], &chunkTiles[0]);
    chunkRenderer.updateSubset(2, &chunkOffsets[2], nullptr);
    chunkRenderer.updateSubset(3, &chunkOffsets[3], nullptr);

    bool quit = false;
    float dt = 1.0f;

    while (!quit) {
        UserInput input = sdl.processEvents();

        playerCameraController.handleInputs(cameraPlayer, input, dt);
        screenCameraController.handleInputs(cameraScreen, input);
        debugOverlayController.handleInputs(debugOverlay, input);



        std::string strX = "";
        std::string strY = "";
        float screenX = input.mouse.x;
        float screenY = input.mouse.y;
        float screenW = input.window.width;
        float screenH = input.window.height;
        glm::vec3 camera = cameraPlayer.getPosition();
        glm::vec2 world = cameraPlayer.screenToWorld(screenX, screenY, screenW, screenH);
        glm::vec2 chunk = WorldToChunk(world);
        glm::vec2 tile = WorldToTile(world);

        strX = std::to_string(camera.x);
        strY = std::to_string(camera.y);
        lines[0].text = "Camera.XY(World): (" + strX + ", " + strY + ")";

        strX = std::to_string(screenX);
        strY = std::to_string(screenY);
        lines[1].text = "Mouse.XY(Screen): (" + strX + ", " + strY + ")";

        strX = std::to_string(world.x);
        strY = std::to_string(world.y);
        lines[2].text = "Mouse.XY(World): (" + strX + ", " + strY + ")";

        strX = std::to_string((int)chunk.x);
        strY = std::to_string((int)chunk.y);
        lines[3].text = "Chunk.XY: (" + strX + ", " + strY + ")";

        strX = std::to_string((int)tile.x);
        strY = std::to_string((int)tile.y);
        lines[4].text = "Tile.XY: (" + strX + ", " + strY + ")";




        cursor.update(cameraPlayer, input);



        sdl.clear();
        chunkRenderer.render(cameraPlayer);
        cursor.render(cameraPlayer);
        debugOverlay.render(cameraScreen, lines);
        sdl.swap();

        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}