#include "config/entity_configs.hpp"
#include "core/application.hpp"
#include "core/entity_manager.hpp"
#include "graphics/render/quad_renderer.hpp"

#include <vector>

#include <iostream>

int main() {

    Application app;
    EntityManager em;
    QuadRenderer qr;
    
    em.spawnEntity(EntityType::RED_QUAD,    {32.0f, 96.0f});
    em.spawnEntity(EntityType::GREEN_QUAD,  {96.0f, 96.0f});
    em.spawnEntity(EntityType::BLUE_QUAD,   {32.0f, 32.0f});
    em.spawnEntity(EntityType::YELLOW_QUAD, {96.0f, 32.0f});
    // em.remove(1);

    const Camera& PLAYER_CAMERA = app.getPlayerCamera();

    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        em.update();
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