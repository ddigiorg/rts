// Types.hpp
#pragma once

namespace Core {

constexpr char* DEFAULT_WINDOW_TITLE = "Game";
constexpr unsigned int DEFAULT_WINDOW_WIDTH = 800;
constexpr unsigned int DEFAULT_WINDOW_HEIGHT = 600;
constexpr unsigned int DEFAULT_MIN_WINDOW_WIDTH = 320;
constexpr unsigned int DEFAULT_MIN_WINDOW_HEIGHT = 200;

struct GameEvents {
    bool quit = false;
    bool windowResize = false;
};

} // namespace Core