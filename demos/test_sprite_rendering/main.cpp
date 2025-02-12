#include "engine/core/sdl_manager.hpp"
#include "engine/gfx/camera.hpp"

#include "game/gfx/sprites_renderer.hpp"
#include "game/input/player_camera_controller.hpp"
#include "game/input/screen_camera_controller.hpp"

#include <string>

using namespace Core;
using namespace GFX;

struct SpriteTransform {
    float x, y, w, h;
};

struct SpriteTexCoord {
    float u, v, w, h;
};

int main() {

    // setup sdl
    SDLManager sdl;

    // setup cameras
    Camera playerCamera(Camera::Type::Orthographic, Camera::Mode::Centered);
    Camera screenCamera(Camera::Type::Orthographic, Camera::Mode::ViewPort);

    // setup input controllers
    PlayerCameraController playerCameraController;
    ScreenCameraController screenCameraController;

    // setup game objects
    SpritesRenderer spritesRenderer;
    spritesRenderer.resize(4);

    // red ball
    SpriteTransform transform0{-64.0f, 64.0f, 32.0f, 32.0f};
    SpriteTexCoord texcoord0{0.0f, 0.5f, 0.5f, 0.5f};
    spritesRenderer.appendSubset(1, &transform0, &texcoord0);

    // green ball
    SpriteTransform transform1{64.0f, 64.0f, 32.0f, 32.0f};
    SpriteTexCoord texcoord1{0.5f, 0.5f, 0.5f, 0.5f};
    spritesRenderer.appendSubset(1, &transform1, &texcoord1);

    // blue ball
    SpriteTransform transform2{-64.0f, -64.0f, 32.0f, 32.0f};
    SpriteTexCoord texcoord2{0.0f, 0.0f, 0.5f, 0.5f};
    spritesRenderer.appendSubset(1, &transform2, &texcoord2);

    // yellow ball
    SpriteTransform transform3{64.0f, -64.0f, 32.0f, 32.0f};
    SpriteTexCoord texcoord3{0.5f, 0.0f, 0.5f, 0.5f};
    spritesRenderer.appendSubset(1, &transform3, &texcoord3);

    // setup loop variables
    bool quit = false;
    float dt = 1.0f;

    // loop
    while (!quit) {

        // handle inputs
        UserInput input = sdl.processEvents();
        playerCameraController.handleInputs(playerCamera, input, dt);
        screenCameraController.handleInputs(screenCamera, input);

        // handle updates

        // handle renders
        sdl.clear();
        spritesRenderer.render(playerCamera);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}