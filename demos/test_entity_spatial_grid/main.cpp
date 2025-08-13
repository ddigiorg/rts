#include "core/application.hpp"
#include "core/entity_spatial_grid.hpp"
#include "graphics/render/quad_renderer.hpp"

int main() {

    // Setup application
    Application app;
    QuadRenderer qr;
    SpatialGrid grid;

    grid.insertEntity(0, {32.0f, 96.0f}, {{-16.0f, -16.0f}, {16.0f, 16.0f}}, {1.0f, 0.0f, 0.0f, 1.0f});
    grid.insertEntity(1, {96.0f, 96.0f}, {{-16.0f, -16.0f}, {16.0f, 16.0f}}, {0.0f, 1.0f, 0.0f, 1.0f});
    grid.insertEntity(2, {32.0f, 32.0f}, {{-16.0f, -16.0f}, {16.0f, 16.0f}}, {0.0f, 0.0f, 1.0f, 1.0f});
    grid.insertEntity(3, {96.0f, 32.0f}, {{-16.0f, -16.0f}, {16.0f, 16.0f}}, {1.0f, 1.0f, 0.0f, 1.0f});

    grid.removeEntity(1);
    grid.removeEntity(3);

    grid.translateEntity(0, {160.0f, 96.0f});

    grid.updateRenderData(qr);

    const Camera& PLAYER_CAMERA = app.getPlayerCamera();

    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        app.handleRenderPre();
        app.handleRender();
        qr.renderOutline(PLAYER_CAMERA);
        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}