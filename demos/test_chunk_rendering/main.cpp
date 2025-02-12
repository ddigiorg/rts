#include "engine/core/sdl_manager.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/ui/cursor.hpp"
#include "engine/ui/debug_overlay.hpp"

#include "game/gfx/chunk_renderer.hpp"
#include "game/input/player_camera_controller.hpp"
#include "game/input/screen_camera_controller.hpp"
#include "game/input/debug_overlay_controller.hpp"

#include <string>

using namespace Core;
using namespace GFX;
using namespace UI;

inline void _setupChunkRenderer(ChunkRenderer& r) {
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

    r.updateSubset(0, &chunkOffsets[0], nullptr);
    r.updateSubset(1, &chunkOffsets[1], &chunkTiles[0]);
    r.updateSubset(2, &chunkOffsets[2], nullptr);
    r.updateSubset(3, &chunkOffsets[3], nullptr);
}

inline void _setupDebugOverlayLines(std::vector<DebugOverlayLine>& lines) {
    lines = {
            DebugOverlayLine("Camera.XY(World): ", {5.0f, -20.0f}),
            DebugOverlayLine("Mouse.XY(Screen): ", {5.0f, -40.0f}),
            DebugOverlayLine("Mouse.XY(World): ", {5.0f, -60.0f}),
            DebugOverlayLine("Chunk.XY: ", {5.0f, -80.0f}),
            DebugOverlayLine("Tile.XY: ", {5.0f, -100.0f}),
        };
}

inline void _updateDebugOverlayLines(
        const Camera& camera,
        const UserInput& input,
        std::vector<DebugOverlayLine>& lines
) {
    // get input information
    float mouseX = input.mouse.x;
    float mouseY = input.mouse.y;
    float windowW = input.window.width;
    float windowH = input.window.height;

    // get object positions
    glm::vec3 posCamera = camera.getPosition();
    glm::vec2 posWorld = camera.screenToWorld(mouseX, mouseY, windowW, windowH);
    glm::vec2 posChunk = WorldToChunk(posWorld);
    glm::vec2 posTile = WorldToTile(posWorld);

    // convert chunk/tile positions to integers
    int chunkX = (int)posChunk.x;
    int chunkY = (int)posChunk.y;
    int tileX = (int)posTile.x;
    int tileY = (int)posTile.y;

    // update lines
    lines[0].text = "Camera.XY(World): (" + std::to_string(posCamera.x) + ", " + std::to_string(posCamera.y) + ")";
    lines[1].text = "Mouse.XY(Screen): (" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + ")";
    lines[2].text = "Mouse.XY(World): (" + std::to_string(posWorld.x) + ", " + std::to_string(posWorld.y) + ")";
    lines[3].text = "Chunk.XY: (" + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ")";
    lines[4].text = "Tile.XY: (" + std::to_string(tileX) + ", " + std::to_string(tileY) + ")";
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
    Cursor cursor;
    DebugOverlay debugOverlay(true);
    std::vector<DebugOverlayLine> lines;
    _setupDebugOverlayLines(lines);

    // setup game objects
    ChunkRenderer chunkRenderer(4);
    _setupChunkRenderer(chunkRenderer);

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
        cursor.update(playerCamera, input);

        // handle renders
        sdl.clear();
        chunkRenderer.render(playerCamera);
        cursor.render(playerCamera);
        debugOverlay.render(screenCamera, lines);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}