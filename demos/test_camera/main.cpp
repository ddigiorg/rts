#include "core/sdl_manager.hpp"
#include "graphics/camera.hpp"

#include <iostream>

class Box {

private:

};

int main() {
    std::cout << "Camera test running..." << std::endl;

    Core::SDLManager sdl;
    Core::EventState events;
    GFX::Camera camera;

    while (!events.quit) {
        sdl.processEvents(events);

        if (events.window.resized) {
            camera.resize(events.window.width, events.window.height);
        }

        sdl.clear();
        sdl.swap();
    }

    return 0;
}