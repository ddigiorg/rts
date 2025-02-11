#pragma once

#include "engine/gfx/camera.hpp"
#include "engine/gfx/font.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace UI {

enum class AnchorX {
    Left, Middle, Right
};

enum class AnchorY {
    Bottom, Middle, Top
};

struct DebugOverlayLine {
    std::string text;
    glm::vec2 position;
    glm::vec3 color;
    unsigned int pt;
    AnchorY anchorY;
    AnchorX anchorX;

    DebugOverlayLine(
        std::string text = "",
        glm::vec2 position = {0.0f, 0.0f},
        glm::vec3 color = {1.0f, 1.0f, 1.0f},
        unsigned int pt = 12,
        AnchorY anchorY = AnchorY::Top,
        AnchorX anchorX = AnchorX::Left
    ) : text(text), position(position), color(color), pt(pt), anchorY(anchorY), anchorX(anchorX) {};
};

class DebugOverlay {
public:
    DebugOverlay(bool isVisible = false);
    void toggleVisible();
    void render(const GFX::Camera& camera, const std::vector<DebugOverlayLine>& lines);

private:
    bool isVisible = false;
    GFX::Font font;
};

DebugOverlay::DebugOverlay(bool isVisible) 
    : isVisible(isVisible) {}

void DebugOverlay::toggleVisible() {
    isVisible = !isVisible;
}

void DebugOverlay::render(const GFX::Camera& camera, const std::vector<DebugOverlayLine>& lines) {
    if (!isVisible) return;

    const glm::mat4 vp = camera.getViewProjMatrix();
    const GFX::Frustum frustum = camera.getFrustum();

    for (const DebugOverlayLine& line : lines) {
        float x, y;

        switch (line.anchorX) {
            case AnchorX::Left:   x = frustum.l + line.position.x; break;
            case AnchorX::Middle: x = (frustum.l + frustum.r) * 0.5f + line.position.x; break;
            case AnchorX::Right:  x = frustum.r + line.position.x; break;
        }

        switch (line.anchorY) {
            case AnchorY::Bottom: y = frustum.b + line.position.y; break;
            case AnchorY::Middle: y = (frustum.b + frustum.t) * 0.5f + line.position.y; break;
            case AnchorY::Top:    y = frustum.t + line.position.y; break;
        }

        font.render(line.text, x, y, line.pt, line.color, vp);
    }
}

} // namespace UI
