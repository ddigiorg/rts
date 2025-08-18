#include "core/application.hpp"
#include "core/entity_manager.hpp"
// #include "core/chunk.hpp"
#include "graphics/render/quad_renderer.hpp"

#include <vector>

int main() {

    Application app;
    EntityManager em;
    QuadRenderer qr;

    em.createEntity(EntityType::RED,    {32.0f, 96.0f});
    em.createEntity(EntityType::GREEN,  {96.0f, 96.0f});
    em.createEntity(EntityType::BLUE,   {32.0f, 32.0f});
    em.createEntity(EntityType::YELLOW, {96.0f, 32.0f});

    const Camera& PLAYER_CAMERA = app.getPlayerCamera();

    while (app.isRunning()) {
        app.handleInput();
        em.updateEntities();
        app.handleUpdate();
        qr.reset();
        em.updateRenderData(qr);
        app.handleRenderPre();
        qr.renderOutline(PLAYER_CAMERA);
        app.handleRender();
        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}