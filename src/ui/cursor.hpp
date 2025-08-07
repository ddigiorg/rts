#pragma once

#include "core/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

// shader filepaths
constexpr const char* CURSOR_SELECT_VERT = "assets/shaders/cursor-select.vert";
constexpr const char* CURSOR_SELECT_FRAG = "assets/shaders/cursor-select.frag";

// cursor constants
constexpr glm::vec4 CURSOR_BOX_SELECT_COLOR = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

class Cursor {
public:
    Cursor();
    ~Cursor();

    void update(FrameState& frame);
    void render(Camera& camera);

    bool getIsSelecting() const { return isSelecting; };

private:
    void _setupBoxSelectPipeline();
    void _updateVertices();
    void _updateSelectEvent(SelectEvent& select);

    bool isSelecting = false;
    bool mouseLPrev = false;

    float boxBegX = 0.0f;
    float boxBegY = 0.0f;
    float boxEndX = 0.0f;
    float boxEndY = 0.0f;

    // box select pipeline
    unsigned int boxSelectVAO = 0;
    unsigned int boxSelectVBO = 0;
    Shader boxSelectShader;
};

Cursor::Cursor() {
    _setupBoxSelectPipeline();
    _updateVertices();
}

Cursor::~Cursor() {
    if (boxSelectVAO) glDeleteVertexArrays(1, &boxSelectVAO);
    if (boxSelectVBO) glDeleteBuffers(1, &boxSelectVBO);
}

void Cursor::_setupBoxSelectPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &boxSelectVAO);
    glGenBuffers(1, &boxSelectVBO);

    // bind vertex array object
    glBindVertexArray(boxSelectVAO);

    // setup box selector vertices
    glBindBuffer(GL_ARRAY_BUFFER, boxSelectVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    boxSelectShader.load(CURSOR_SELECT_VERT, CURSOR_SELECT_FRAG);
    boxSelectShader.use();
    float r = CURSOR_BOX_SELECT_COLOR.r;
    float g = CURSOR_BOX_SELECT_COLOR.g;
    float b = CURSOR_BOX_SELECT_COLOR.b;
    float a = CURSOR_BOX_SELECT_COLOR.a;
    boxSelectShader.setUniform4f("uColor", r, g, b, a); // TODO: pass vec4 directly into setUniform4f
}

void Cursor::_updateVertices() {
    float vertices[8] = {
        boxBegX, boxBegY,
        boxEndX, boxBegY,
        boxEndX, boxEndY,
        boxBegX, boxEndY
    };

    glBindBuffer(GL_ARRAY_BUFFER, boxSelectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void Cursor::_updateSelectEvent(SelectEvent& select) {
    select.isActive = true;
    select.boxBegX = boxBegX;
    select.boxBegY = boxBegY;
    select.boxEndX = boxEndX;
    select.boxEndY = boxEndY;
}

void Cursor::update(FrameState& frame) {
    const WindowInput& window = frame.input.window;
    const MouseInput& mouse = frame.input.mouse;

    bool mouseL = (mouse.buttons & SDL_BUTTON_LMASK) != 0;

    // start selecting
    if (mouseL && !mouseLPrev) {
        isSelecting = true;
        boxBegX = mouse.x;
        boxBegY = mouse.y;
        boxEndX = boxBegX;
        boxEndY = boxBegY;
        _updateVertices();
    }

    // update selection box
    if (isSelecting) {
        boxEndX = mouse.x;
        boxEndY = mouse.y;
        _updateVertices();
    }

    // stop selecting
    if (!mouseL && mouseLPrev) {
        isSelecting = false;
        _updateSelectEvent(frame.events.select);
    }

    mouseLPrev = mouseL;
    frame.states.isSelecting = isSelecting;
}

void Cursor::render(Camera& camera) {
    if (!isSelecting) return;

    boxSelectShader.use();
    boxSelectShader.setUniformMatrix4fv("uVP", 1, camera.getViewProjMatrix());

    glLineWidth(2.0f);
    glBindVertexArray(boxSelectVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glLineWidth(1.0f);
    glBindVertexArray(0);
}
