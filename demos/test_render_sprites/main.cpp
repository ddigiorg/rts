#include "core/application.hpp"
#include "core/types.hpp"
#include "graphics/render/sprite_renderer.hpp"

// #include <vector>

int main() {

    // Setup application
    Application app;

    // Setup sprite renderer
    size_t capacity = 4;
    size_t count = 4;
    SpriteRenderer spriteRenderer;
    spriteRenderer.setCapacity(capacity);
    spriteRenderer.setCount(count);

    // TODO: STANDARDIZE THESE
    std::vector<Position3f> positions(count);
    std::vector<Position2f> scales(count);
    std::vector<TexCoord4f> texcoords(count);

    // sprite: terrain grass
    positions[0] = Position3f{0.0f, 0.0f, 0.0f};
    scales[0] = Position2f{32.0f, 32.0f};
    texcoords[0] = computeTexCoord( 64, 128,  64, 64);

    // // sprite: terrain water
    // positions[1] = Position{32.0f, 0.0f, 0.0f};
    // scales[1] = Scale{32.0f, 32.0f};
    // texcoords[1] = SPRITE_TEXCOORDS[SPRITE_TERRAIN_WATER];

    // // sprite: unit cyan
    // positions[2] = Position{0.0f, 0.0f, 0.0f};
    // scales[2] = Scale{32.0f, 32.0f};
    // texcoords[2] = SPRITE_TEXCOORDS[SPRITE_UNIT_CYAN_270];

    // // sprite: unit magenta
    // positions[3] = Position{32.0f, 0.0f, 0.0f};
    // scales[3] = Scale{32.0f, 32.0f};
    // texcoords[3] = SPRITE_TEXCOORDS[SPRITE_UNIT_MAGENTA_270];

    spriteRenderer.updatePositionData(0, count, &positions[0]);
    spriteRenderer.updateScaleData(0, count, &scales[0]);
    spriteRenderer.updateTexCoordData(0, count, &texcoords[0]);

    const Camera& playerCamera = app.getPlayerCamera();

    while (app.isRunning()) {
        app.handleInput();
        app.handleUpdate();
        app.handleRenderPre();
        app.handleRender();
        spriteRenderer.render(playerCamera);
        app.handleRenderPost();
        app.handleQuit();
    }

    return 0;
}