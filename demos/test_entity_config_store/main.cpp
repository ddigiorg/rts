#include "common/config.hpp"
#include "core/application.hpp"
#include "core/entity_manager.hpp"
#include "core/map.hpp"
#include "graphics/render/quad_renderer.hpp"

#include <vector>

#include <iostream>

int main() {

    Application app;
    Map map;
    EntityManager em;
    QuadRenderer qr;
    
    em.spawn(EntityType::RED,    {32.0f, 96.0f});
    em.spawn(EntityType::GREEN,  {96.0f, 96.0f});
    em.spawn(EntityType::BLUE,   {32.0f, 32.0f});
    em.spawn(EntityType::YELLOW, {96.0f, 32.0f});
    // em.remove(1);

    const Camera& PLAYER_CAMERA = app.getPlayerCamera();

    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        em.update();
        qr.reset();
        map.updateRenderData(qr);
        em.updateRenderData(qr);
        app.handleRenderPre();
        qr.renderOutline(PLAYER_CAMERA);
        app.handleRender();

        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}