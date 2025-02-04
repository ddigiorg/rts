#pragma once

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <cstdint>

// NOTE: references for SDL3 states:
// - mouse states: https://wiki.libsdl.org/SDL3/SDL_MouseButtonFlags
// - keyboard states: https://wiki.libsdl.org/SDL3/SDL_Scancode

namespace Core {

struct InputState {
    bool quit = false;

    struct Window {
        bool resized = false;
        int width = 0;
        int height = 0;
    } window;

    struct Mouse {
        bool moved = false;
        float x = 0.0f;
        float y = 0.0f;
        uint32_t buttons;
        struct Wheel {
            bool moved = false;
            float x = 0.0f;
            float y = 0.0f;
        } wheel;
    } mouse;

    struct Keyboard {
        int numkeys = 0;
        const bool* buttons = nullptr;
    } keyboard;
};

} // namespace Core