#pragma once

#include "core/types.hpp"
#include "core/input_state.hpp"
#include "graphics/camera.hpp"
#include "graphics/font.hpp"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace GFX {

struct DebugScreenLine {
    std::string text = "";
    float x = 0.0f;
    float y = 0.0f;
    unsigned int pt = 10;
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
};

class DebugScreen {
public:
    DebugScreen(bool isVisible = false);
    void update(const Core::InputState& input);
    void render(const std::vector<DebugScreenLine>& lines);

    bool isActive() const { return isVisible; };

private:
    void _resize(int width, int height);

    bool isVisible = false;
    bool pressedPrev = false;

    Font font;

    glm::mat4x4 vMat;  // view matrix
    glm::mat4x4 pMat;  // projection matrix
    glm::mat4x4 vpMat; // view projection matrix
};

DebugScreen::DebugScreen(bool isVisible) {
    this->isVisible = isVisible;

    // setup view matrix
    glm::vec3 position = {0.0f, 0.0f, 1.0f};
    glm::vec3 front = {0.0f, 0.0f, -1.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    vMat = glm::lookAt(position, position + front, up);

    // setup projection matrix
    _resize(Core::DEFAULT_WINDOW_WIDTH, Core::DEFAULT_WINDOW_HEIGHT);

    // setup view projection matrix
    vpMat = pMat * vMat;
}

void DebugScreen::update(const Core::InputState& input) {

    if (input.window.resized)
        _resize(input.window.width, input.window.height);

    // TODO: improve this
    bool pressed = input.keyboard.buttons[SDL_SCANCODE_GRAVE];
    if (pressed && !pressedPrev)
        isVisible = !isVisible;
    pressedPrev = pressed;
}

void DebugScreen::render(const std::vector<DebugScreenLine>& lines) {
    if (!isVisible)
        return;

    for (const DebugScreenLine& line : lines) {
        font.render(line.text, line.x, line.y, line.pt, line.color, vpMat);
    }
}

void DebugScreen::_resize(int width, int height) {
    float left   = 0.0f;
    float right  = (float)width;
    float bottom = 0.0f;
    float top    = (float)height;
    float near   = -1.0f;
    float far    = 1.0f;
    pMat = glm::ortho(left, right, bottom, top, near, far);
    vpMat = pMat * vMat;
}

} // namespace GFX

