#pragma once

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <cstdint>

// NOTE: references for SDL3 states:
// - mouse states: https://wiki.libsdl.org/SDL3/SDL_MouseButtonFlags
// - keyboard states: https://wiki.libsdl.org/SDL3/SDL_Scancode

namespace Input {

struct WindowInput {
    bool resized = false;
    int width = 0;
    int height = 0;
};

struct MouseWheelInput {
    bool moved = false;
    float x = 0.0f;
    float y = 0.0f;
};

struct MouseInput {
    bool moved = false;
    float x = 0.0f;
    float y = 0.0f;
    float xrel = 0.0f;
    float yrel = 0.0f;
    uint32_t buttons;
    MouseWheelInput wheel;
};

struct KeyboardInput {
    int numkeys = 0;
    const bool* buttons = nullptr;
};

struct UserInput {
    bool quit = false;
    WindowInput window;
    MouseInput  mouse;
    KeyboardInput keyboard;
};

} // namespace Input