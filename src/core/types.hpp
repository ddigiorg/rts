#pragma once

#include <unordered_map>

namespace Core {

constexpr char* DEFAULT_WINDOW_TITLE = "Game";
constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 600;
constexpr int DEFAULT_MIN_WINDOW_WIDTH = 320;
constexpr int DEFAULT_MIN_WINDOW_HEIGHT = 200;

struct ButtonState {
    bool pressed = false;
    bool released = false;
};

struct WindowInput {
    bool resized = false;
    int width = DEFAULT_WINDOW_WIDTH;
    int height = DEFAULT_WINDOW_HEIGHT;
};

struct MouseInput {
    bool moved = false;
    int x = 0;
    int y = 0;
    std::unordered_map<unsigned int, ButtonState> buttons;
};

struct KeyboardInput {
    std::unordered_map<unsigned int, ButtonState> buttons;
};

struct FrameInput {
    bool quit = false;
    WindowInput window;
    MouseInput mouse;
    KeyboardInput keyboard;
};

} // namespace Core