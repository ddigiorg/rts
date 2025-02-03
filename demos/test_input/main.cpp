#include "core/sdl_manager.hpp"

#include <iostream>

int main( int argc, char* args[] ) {
    Core::SDLManager sdl;

    bool quit = false;
    while (!quit) {
        Core::FrameInput input = sdl.processEvents();
        quit = input.quit;

        // // mouse buttons
        // if (input.mouse.buttons[1].pressed)
        //     std::cout << "Mouse button 1 pressed." << std::endl;
        // if (input.mouse.buttons[1].released)
        //     std::cout << "Mouse button 1 released." << std::endl;
        // if (input.mouse.buttons[2].pressed)
        //     std::cout << "Mouse button 2 pressed." << std::endl;
        // if (input.mouse.buttons[2].released)
        //     std::cout << "Mouse button 2 released." << std::endl;
        // if (input.mouse.buttons[3].pressed)
        //     std::cout << "Mouse button 3 pressed." << std::endl;
        // if (input.mouse.buttons[3].released)
        //     std::cout << "Mouse button 3 released." << std::endl;
        // if (input.mouse.buttons[4].pressed)
        //     std::cout << "Mouse button 4 pressed." << std::endl;
        // if (input.mouse.buttons[4].released)
        //     std::cout << "Mouse button 4 released." << std::endl;
        // if (input.mouse.buttons[5].pressed)
        //     std::cout << "Mouse button 5 pressed." << std::endl;
        // if (input.mouse.buttons[5].released)
        //     std::cout << "Mouse button 5 released." << std::endl;

        // // keyboard left, right, up, down buttons
        // if (input.keyboard.buttons[SDLK_LEFT].pressed)
        //     std::cout << "Keyboard button left pressed." << std::endl;
        // if (input.keyboard.buttons[SDLK_LEFT].released)
        //     std::cout << "Keyboard button left released." << std::endl;
        // if (input.keyboard.buttons[SDLK_RIGHT].pressed)
        //     std::cout << "Keyboard button right pressed." << std::endl;
        // if (input.keyboard.buttons[SDLK_RIGHT].released)
        //     std::cout << "Keyboard button right released." << std::endl;
        // if (input.keyboard.buttons[SDLK_UP].pressed)
        //     std::cout << "Keyboard button up pressed." << std::endl;
        // if (input.keyboard.buttons[SDLK_UP].released)
        //     std::cout << "Keyboard button up released." << std::endl;
        // if (input.keyboard.buttons[SDLK_DOWN].pressed)
        //     std::cout << "Keyboard button down pressed." << std::endl;
        // if (input.keyboard.buttons[SDLK_DOWN].released)
        //     std::cout << "Keyboard button down released." << std::endl;

        // // keyboard buttons
        // if (input.keyboard.buttons[SDLK_ESCAPE].pressed)
        //     std::cout << "Keyboard button escape key pressed." << std::endl;
        // if (input.keyboard.buttons[SDLK_ESCAPE].released)
        //     std::cout << "Keyboard button escape key released." << std::endl;
        // if (input.keyboard.buttons[SDLK_BACKQUOTE].pressed)
        //     std::cout << "Keyboard button backquote key pressed." << std::endl;
        // if (input.keyboard.buttons[SDLK_BACKQUOTE].released)
        //     std::cout << "Keyboard button backquote key released." << std::endl;

        sdl.clear();
        sdl.swap();
    }

    return 0;
}