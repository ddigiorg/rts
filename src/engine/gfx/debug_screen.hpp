#pragma once

#include "engine/gfx/camera.hpp"
#include "engine/gfx/font.hpp"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace GFX {

enum class AnchorX {
    Left, Middle, Right
};

enum class AnchorY {
    Bottom, Middle, Top
};

struct DebugScreenLine {
    std::string text = "";
    AnchorX anchorX = AnchorX::Middle;
    AnchorY anchorY = AnchorY::Middle;
    glm::vec2 position = {0.0f, 0.0f};
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    unsigned int pt = 10;
};

class DebugScreen {
public:
    DebugScreen(bool isVisible = false);
    void toggleVisible();
    void render(Camera& camera, const std::vector<DebugScreenLine>& lines);

private:
    bool isVisible = false;
    Font font;
};

DebugScreen::DebugScreen(bool isVisible) {
    this->isVisible = isVisible;
}

void DebugScreen::toggleVisible() {
    isVisible = !isVisible;
}

void DebugScreen::render(Camera& camera, const std::vector<DebugScreenLine>& lines) {
    if (!isVisible)
        return;

    Frustum frustum = camera.getFrustum();
    glm::mat4 cameraVP = camera.getViewProjMatrix();

    for (const DebugScreenLine& line : lines) {
        float x = 0.0f;
        float y = 0.0f;

        if (line.anchorX == AnchorX::Left)
            x = frustum.l + line.position.x;
        else if (line.anchorX == AnchorX::Middle)
            x = ((frustum.r - frustum.l) / 2.0f) + line.position.x;
        else if (line.anchorX == AnchorX::Right)
            x = frustum.r + line.position.x;

        if (line.anchorY == AnchorY::Bottom)
            y = frustum.b + line.position.y;
        else if (line.anchorY == AnchorY::Middle)
            y = ((frustum.t - frustum.b) / 2.0f) + line.position.y;
        else if (line.anchorY == AnchorY::Top)
            y = frustum.t + line.position.y;

        font.render(line.text, x, y, line.pt, line.color, cameraVP);
    }
}

} // namespace GFX

