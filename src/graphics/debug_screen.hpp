#pragma once

#include "core/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/font.hpp"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <string>

struct DebugData {
    int fps = 0;
    int cameraX = 0;
    int cameraY = 0;
    int mouseX = 0;
    int mouseY = 0;
    int mouseWorldX = 0;
    int mouseWorldY = 0;
};

struct DebugRenderLine {
    std::string text;
    float x, y;
};

namespace GFX {

class DebugScreen {
public:
    void handleEvents(const Core::FrameInput& input);
    void update(const DebugData& data);
    void render(Camera& camera);

private:
    bool isVisible = false;
    unsigned int pt = 10;
    glm::vec3 color = {1.0f, 1.0f, 1.0f};

    DebugData data;
    Font font;
};

void DebugScreen::handleEvents(const Core::FrameInput& input) {
    if (input.keyboard.buttons.at(SDLK_BACKQUOTE).pressed) {
        isVisible = !isVisible;
    }
}

void DebugScreen::update(const DebugData& data) {
    this->data = data;
}

void DebugScreen::render(Camera& camera) {
    if (!isVisible)
        return;

    glm::vec2 aspect = camera.getAspect();

    float x = -aspect.x + 5.0f;
    float y = aspect.y - 20.0f;
    DebugRenderLine lines[4];

    lines[0].text = "FPS: " + std::to_string(data.fps);
    lines[0].x = x;
    lines[0].y = y;

    lines[1].text = "Camera: (" + std::to_string(data.cameraX) + ", " + std::to_string(data.cameraY) + ")";
    lines[1].x = x;
    lines[1].y = y - 20.f;

    lines[2].text = "Mouse (Screen): (" + std::to_string(data.mouseX) + ", " + std::to_string(data.mouseY) + ")";
    lines[2].x = x;
    lines[2].y = y - 40.f;

    lines[3].text = "Mouse (World): (" + std::to_string(data.mouseWorldX) + ", " + std::to_string(data.mouseWorldY) + ")";
    lines[3].x = x;
    lines[3].y = y - 60.f;

    for (const DebugRenderLine& line : lines)
        font.render(line.text, line.x, line.y, pt, color, camera.getViewProjMatrix());
}

} // namespace GFX

