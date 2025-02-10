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
        "Camera.XY(World): ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -20.0f}
    };

    DebugScreenLine line1 = {
        "Mouse.XY(Screen): ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -40.0f}
    };

    DebugScreenLine line2 = {
        "Mouse.XY(World): ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -60.0f}
    };

    DebugScreenLine line3 = {
        "Chunk.XY: ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -80.0f}
    };

    DebugScreenLine line4 = {
        "Tile.XY: ",
        AnchorX::Left, AnchorY::Top,
        {5.0f, -100.0f}
    };

    std::vector<DebugScreenLine> lines = {line0, line1, line2, line3, line4};



    std::vector<glm::vec2> chunkOffsets = {
        ChunkToWorld({0, 0}),
        ChunkToWorld({1, 0}),
        ChunkToWorld({0, 1}),
        ChunkToWorld({1, 1})
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
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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

        ctrlCameraPlayer.handleInputs(cameraPlayer, input, dt);
        ctrlCameraScreen.handleInputs(cameraScreen, input);
        ctrlDebugScreen.handleInputs(debugScreen, input);

        glm::vec3 pos = cameraPlayer.getPosition();
        std::string strCamX = std::to_string(pos.x);
        std::string strCamY = std::to_string(pos.y);
        lines[0].text = "Camera.XY(World): (" + strCamX + ", " + strCamY + ")";

        if (input.mouse.moved) {
            std::string strX = "";
            std::string strY = "";
            float screenX = input.mouse.x;
            float screenY = input.mouse.y;
            float screenW = input.window.width;
            float screenH = input.window.height;
            glm::vec2 world = cameraPlayer.screenToWorldCoords(screenX, screenY, screenW, screenH);
            glm::vec2 chunk = WorldToChunk(world);
            glm::vec2 tile = WorldToTile(world);

            strX = std::to_string(screenX);
            strY = std::to_string(screenY);
            lines[1].text = "Mouse.XY(Screen): (" + strX + ", " + strY + ")";

            strX = std::to_string(world.x);
            strY = std::to_string(world.y);
            lines[2].text = "Mouse.XY(World): (" + strX + ", " + strY + ")";

            strX = std::to_string(chunk.x);
            strY = std::to_string(chunk.y);
            lines[3].text = "Chunk.XY: (" + strX + ", " + strY + ")";

            strX = std::to_string(tile.x);
            strY = std::to_string(tile.y);
            lines[4].text = "Tile.XY: (" + strX + ", " + strY + ")";
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