#include "core/sdl_manager.hpp"
#include "graphics/debug_screen.hpp"

#include "glm/vec3.hpp"

#include <string>
#include <iostream>

int main() {
    Core::SDLManager sdl;
    GFX::Camera cameraScreen;
    GFX::DebugScreen debugScreen(true);

    float windowHeight = (float)sdl.getWindowHeight();

    // setup debug screen lines
    std::vector<GFX::DebugScreenLine> lines;
    std::string text;
    float x;
    float y;
    unsigned int pt;
    glm::vec3 color;

    // setup debug screen line 1
    text = "Mouse Position:";
    x = 5.0f;
    y = windowHeight - 20.0f;
    pt = 12;
    color = {1.0f, 1.0f, 1.0f};
    lines.push_back({text, x, y, pt, color});

    // setup debug screen line 2
    text = "TEST";
    x = 5.0f;
    y = windowHeight - 40.0f;
    pt = 12;
    color = {0.0f, 1.0f, 0.0f};
    lines.push_back({text, x, y, pt, color});

    // loop
    bool quit = false;
    while (!quit) {

        // process events
        Core::InputState input = sdl.processEvents();

        // update lines dynamically
        if (debugScreen.isActive()) {
            std::string strX = std::to_string((int)input.mouse.x);
            std::string strY = std::to_string((int)input.mouse.y);
            lines[0].text = "Mouse Position: (" + strX + ", " + strY + ")";
        }

        // update
        debugScreen.update(input);

        // render
        sdl.clear();
        debugScreen.render(lines);
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}