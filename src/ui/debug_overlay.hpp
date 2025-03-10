#pragma once

#include "core/types.hpp"
#include "core/frame_state.hpp"
#include "core/components/position2i.hpp"
#include "core/components/position3f.hpp"
#include "graphics/camera.hpp"
#include "graphics/font.hpp"
#include "input/user_input.hpp"

#include <glm/glm.hpp>

#include <string>
#include <vector>

using namespace Core;
using namespace Graphics;
using namespace Input;

namespace UI {

struct DebugOverlayLine {
    std::string text;
    glm::vec2 position;
    glm::vec3 color;
    unsigned int pt;

    DebugOverlayLine(
        std::string text = "",
        unsigned int pt = 12,
        glm::vec2 position = {0.0f, 0.0f},
        glm::vec3 color = {1.0f, 1.0f, 1.0f}
    ) : text(text), pt(pt), position(position), color(color) {};
};

class DebugOverlay {
public:
    DebugOverlay(bool isActive = false);
    void toggleActive() { isActive = !isActive; };
    void update(const Camera& camera, FrameState& frame);
    void render(const Camera& camera);

private:
    bool isActive = false;
    Graphics::Font font;
    std::vector<DebugOverlayLine> lines;
};

DebugOverlay::DebugOverlay(bool isActive) : isActive(isActive) {
    unsigned int pt = 12;
    float spacing = pt + 4.0f;
    float x = 5.0f;
    float y = 5.0f;
    lines.push_back(DebugOverlayLine("test", pt, {x, y}));
    lines.push_back(DebugOverlayLine("test", pt, {x, y+=spacing}));
    lines.push_back(DebugOverlayLine("test", pt, {x, y+=spacing}));
    lines.push_back(DebugOverlayLine("test", pt, {x, y+=spacing}));
    lines.push_back(DebugOverlayLine("test", pt, {x, y+=spacing}));
    lines.push_back(DebugOverlayLine("test", pt, {x, y+=spacing}));
}

void DebugOverlay::update(const Camera& camera, FrameState& frame) {
    if (isActive) {
        const WindowInput& window = frame.input.window;
        const MouseInput& mouse = frame.input.mouse;
        const glm::vec3& cameraPos = camera.getPosition();

        // TODO: clean this up
        glm::vec2 mouseWorldPos = camera.screenToWorld(mouse.x, mouse.y, window.width, window.height);
        Position3f pos = Position3f{mouseWorldPos.x, mouseWorldPos.y, 0.0f};
        Position2i mouseIdx = Position2i{(int)pos.x, (int)pos.y};
        Position2i chunkIdx = chunkWorldToGrid(pos);
        Position2i sectorIdx = sectorWorldToGrid(pos);
        Position2i tileIdx = tileWorldToGrid(pos);

        lines[0].text = "Camera.XY(World): (" + std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.y) + ")";
        lines[1].text = "Mouse.XY(Screen): (" + std::to_string(mouseIdx.x) + ", " + std::to_string(mouseIdx.y) + ")";
        lines[2].text = "Mouse.XY(World):  (" + std::to_string(mouseWorldPos.x) + ", " + std::to_string(mouseWorldPos.y) + ")";
        lines[3].text = "Chunk.XY:  (" + std::to_string(chunkIdx.x) + ", " + std::to_string(chunkIdx.y) + ")";
        lines[4].text = "Sector.XY: (" + std::to_string(sectorIdx.x) + ", " + std::to_string(sectorIdx.y) + ")";
        lines[5].text = "Tile.XY:   (" + std::to_string(tileIdx.x) + ", " + std::to_string(tileIdx.y) + ")";
    }

    frame.states.isDebugActive = isActive;
}

void DebugOverlay::render(const Graphics::Camera& camera) {
    if (!isActive) return;

    const glm::mat4& vp = camera.getViewProjMatrix();
    const Frustum& frustum = camera.getFrustum();

    for (const DebugOverlayLine& line : lines) {
        float x = line.position.x;
        float y = frustum.t - (float)line.pt - line.position.y;
        font.render(line.text, x, y, line.pt, line.color, vp);
    }
}

} // namespace UI
