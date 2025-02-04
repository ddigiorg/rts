#include "core/sdl_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/debug_screen.hpp"
#include "graphics/quad_renderer.hpp"

#include <iostream>

struct Quad {
    float position[2];
    float size[1];
    float color[3];
};

int main() {
    Core::SDLManager sdl;
    GFX::Camera camera;
    GFX::QuadRenderer quadRenderer(1);

    Quad quad {
        {0.0f, 0.0f}, // position
        {128.0f}, // size
        {1.0f, 1.0f, 1.0f} // color
    };

    quadRenderer.clear();
    quadRenderer.append(1, &quad.position, &quad.size, &quad.color);

    // loop
    bool quit = false;
    while (!quit) {

        // process events
        Core::InputState input = sdl.processEvents();

        // update
        camera.update(input);

        // render
        sdl.clear();
        quadRenderer.render(camera);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}