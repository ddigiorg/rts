#pragma once

#include "common/config.hpp"
#include "common/types.hpp"

#include <SDL3/SDL.h>

class InputManager {
public:
    InputManager();
    const UserInput& InputManager::processEvents();

private:
    UserInput input;
};

InputManager::InputManager() {
    input.window.width  = WINDOW_DEFAULT_SIZE_X;
    input.window.height = WINDOW_DEFAULT_SIZE_Y;
    input.keyboard.buttons = SDL_GetKeyboardState(&input.keyboard.numkeys);
}

const UserInput& InputManager::processEvents() {
    SDL_Event event;

    // reset flags
    input.window.resized = false;
    input.mouse.moved = false;
    input.mouse.wheel.moved = false;

    // poll for SDL events
    while(SDL_PollEvent(&event)) {

        // mouse motion
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            input.mouse.moved = true;
            input.mouse.x = event.motion.x;
            input.mouse.y = event.motion.y;
            input.mouse.xrel = event.motion.xrel;
            input.mouse.yrel = event.motion.yrel;
        }

        // mouse wheel
        if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            input.mouse.wheel.moved = true;
            input.mouse.wheel.x = event.wheel.x;
            input.mouse.wheel.y = event.wheel.y;
        }

        // window resized
        if(event.type == SDL_EVENT_WINDOW_RESIZED) {
            input.window.resized = true;
            input.window.width = event.window.data1;
            input.window.height = event.window.data2;
            glViewport(0, 0, input.window.width, input.window.height);
        }

        // quit
        if (event.type == SDL_EVENT_QUIT) {
            input.quit = true;
        }
    }

    // update mouse states after polling events
    input.mouse.buttons = SDL_GetMouseState(nullptr, nullptr);

    return input;
}
