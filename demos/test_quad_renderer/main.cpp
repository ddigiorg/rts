#include "core/application.hpp"
#include "core/types.hpp"
#include "graphics/render/quad_renderer.hpp"

#include <vector>

int main() {

    // Setup application
    Application app;
    const Camera& PLAYER_CAMERA = app.getPlayerCamera();

    // Setup quad renderer
    constexpr size_t CAPACITY = 4;
    constexpr size_t COUNT = 4;
    constexpr float LINE_WIDTH = 2.0f;

    QuadRenderer qr;
    qr.setCapacity(CAPACITY);
    qr.setCount(COUNT);

    std::vector<BoundingBox> bboxes(COUNT, BoundingBox(0.0f, 0.0f, 32.0f, 32.0f));
    std::vector<Color4f> colors(COUNT, Color4f(1.0f, 1.0f, 1.0f, 1.0f));

    // Update quad renderer with positions and colors
    bboxes[0].translate({-32.0f,  32.0f});
    colors[0] = {1.0f, 0.0f, 0.0f, 1.0f};

    bboxes[1].translate({ 32.0f,  32.0f});
    colors[1] = {0.0f, 1.0f, 0.0f, 1.0f};

    bboxes[2].translate({-32.0f, -32.0f});
    colors[2] = {0.0f, 0.0f, 1.0f, 1.0f};

    bboxes[3].translate({ 32.0f, -32.0f});
    colors[3] = {1.0f, 1.0f, 0.0f, 1.0f};

    qr.updateBounds(0, COUNT, bboxes.data());
    qr.updateColors(0, COUNT, colors.data());

    // Run application loop
    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        app.handleRenderPre();
        app.handleRender();
        qr.renderOutline(PLAYER_CAMERA, LINE_WIDTH);
        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}