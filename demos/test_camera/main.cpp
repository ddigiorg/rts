#include "core/sdl_manager.hpp"
#include "graphics/camera.hpp"
#include "graphics/quad_renderer.hpp"

#include <iostream>
#include <vector>

int main() {
    std::cout << "Camera test running..." << std::endl;

    Core::SDLManager sdl;
    Core::EventState events;
    GFX::Camera camera;
    GFX::QuadRenderer quadRenderer(1);

    std::vector<float> quadPos = {0.0f, 0.0f};
    std::vector<float> quadSize = {128.0f};
    std::vector<float> quadColor = {1.0f, 1.0f, 1.0f};

    while (!events.quit) {
        sdl.processEvents(events);
        camera.handleEvents(events);

        // update
        camera.update();
        quadRenderer.append(1, quadPos.data(), quadSize.data(), quadColor.data());

        // render
        sdl.clear();
        quadRenderer.render(camera);
        sdl.swap();
    }

    return 0;
}