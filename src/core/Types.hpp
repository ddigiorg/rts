// Types.hpp
#pragma once

namespace Core {

constexpr char* DEFAULT_WINDOW_TITLE = "Game";
constexpr unsigned int DEFAULT_WINDOW_WIDTH = 800;
constexpr unsigned int DEFAULT_WINDOW_HEIGHT = 600;
constexpr unsigned int DEFAULT_MIN_WINDOW_WIDTH = 320;
constexpr unsigned int DEFAULT_MIN_WINDOW_HEIGHT = 200;

struct WindowState {
    bool resized = false;
    int width = DEFAULT_WINDOW_WIDTH;
    int height = DEFAULT_WINDOW_HEIGHT;
};

struct MouseState {
    int posScreenX = 0;
    int posScreenY = 0;
    float posWorldX = 0.0f;
    float posWorldY = 0.0f;
    bool moved = false;
    bool leftPressed = false;
    bool leftReleased = false;
    bool rightPressed = false;
    bool rightReleased = false;
    bool middlePressed = false;
    bool middleReleased = false;
};

struct KeyboardState {
    bool tildePressed = false;
    bool tildeReleased = false;
};

struct EventState {
    bool quit = false;
    WindowState window;
    MouseState mouse;
    KeyboardState keyboard;
};

} // namespace Core