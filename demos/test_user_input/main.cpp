#include "engine/core/sdl_manager.hpp"
#include "engine/core/user_input.hpp"

#include <iostream>

using namespace Core;

int main() {
    SDLManager sdl;

    // loop
    bool quit = false;
    while (!quit) {

        // process events
        UserInput input = sdl.processEvents();

        // mouse motion
        if(input.mouse.moved)
            std::cout << "mouse=(" << input.mouse.x << ", " << input.mouse.y << ")" << std::endl;
        
        // mouse buttons
        if(input.mouse.buttons == SDL_BUTTON_LMASK)
            std::cout << "mouse1" << std::endl;
        if(input.mouse.buttons == SDL_BUTTON_RMASK)
            std::cout << "mouse2" << std::endl;
        if(input.mouse.buttons == SDL_BUTTON_MMASK)
            std::cout << "mouse3" << std::endl;
        if(input.mouse.buttons == SDL_BUTTON_X1MASK)
            std::cout << "mouse4" << std::endl;
        if(input.mouse.buttons == SDL_BUTTON_X2MASK)
            std::cout << "mouse5" << std::endl;

        // mouse wheel
        if(input.mouse.wheel.moved)
            std::cout << "mousewheel=" << input.mouse.wheel.y << std::endl;

        // keyboard buttons
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            std::cout << "escape" << std::endl;
        if(input.keyboard.buttons[SDL_SCANCODE_LSHIFT])
            std::cout << "shift" << std::endl;
        if(input.keyboard.buttons[SDL_SCANCODE_LEFT])
            std::cout << "left" << std::endl;
        if(input.keyboard.buttons[SDL_SCANCODE_RIGHT])
            std::cout << "right" << std::endl;
        if(input.keyboard.buttons[SDL_SCANCODE_UP])
            std::cout << "up" << std::endl;
        if(input.keyboard.buttons[SDL_SCANCODE_DOWN])
            std::cout << "down" << std::endl;

        // render
        sdl.clear();
        sdl.swap();

        // handle quit
        quit = input.quit;
        if(input.keyboard.buttons[SDL_SCANCODE_ESCAPE])
            quit = true;
    }

    return 0;
}