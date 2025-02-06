#pragma once

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <cstdint>

// NOTE: references for SDL3 states:
// - mouse states: https://wiki.libsdl.org/SDL3/SDL_MouseButtonFlags
// - keyboard states: https://wiki.libsdl.org/SDL3/SDL_Scancode

namespace Core {

struct InputWindow {
    bool resized = false;
    int width = 0;
    int height = 0;
};

struct InputMouseWheel {
    bool moved = false;
    float x = 0.0f;
    float y = 0.0f;
};

struct InputMouse {
    bool moved = false;
    float x = 0.0f;
    float y = 0.0f;
    float xrel = 0.0f;
    float yrel = 0.0f;
    uint32_t buttons;
    InputMouseWheel wheel;
};

struct InputKeyboard {
    int numkeys = 0;
    const bool* buttons = nullptr;
};

struct UserInput {
    bool quit = false;
    InputWindow window;
    InputMouse  mouse;
    InputKeyboard keyboard;
};

} // namespace Core