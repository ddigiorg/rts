#include "common/config.hpp"
#include "core/application.hpp"
#include "core/chunk.hpp"
#include "graphics/render/quad_renderer.hpp"

#include <vector>

int main() {

    Application app;
    QuadRenderer qr;

    Chunk chunk({0, 0});

    // em.spawn(EntityType::RED,    {32.0f, 96.0f});
    // em.spawn(EntityType::GREEN,  {96.0f, 96.0f});
    // em.spawn(EntityType::BLUE,   {32.0f, 32.0f});
    // em.spawn(EntityType::YELLOW, {96.0f, 32.0f});

    const Camera& PLAYER_CAMERA = app.getPlayerCamera();

    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        qr.reset();
        // em.updateRenderData(qr);
        app.handleRenderPre();
        qr.renderOutline(PLAYER_CAMERA);
        app.handleRender();
        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}