#pragma once

#include "engine/core/user_input.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/shader.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace GFX {

// shader filepaths
constexpr const char* CURSOR_BOX_SELECT_VERT = "assets/shaders/cursor_box_select_vert.glsl";
constexpr const char* CURSOR_BOX_SELECT_FRAG = "assets/shaders/cursor_box_select_frag.glsl";

// cursor constants
constexpr glm::vec4 CURSOR_BOX_SELECT_COLOR = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

class Cursor {
public:
    Cursor();
    ~Cursor();

    void update(Camera& camera, const Core::UserInput& input);
    void render(Camera& camera);

    bool getIsSelecting() const { return isSelecting; };
    glm::vec2 getBegPos() const { return begPos; };
    glm::vec2 getEndPos() const { return endPos; };

private:
    void _setupBoxSelectPipeline();
    void _updateVertices();

    bool isSelecting = false;
    bool mouseLPrev = false;

    glm::vec2 begPos = {0.0f, 0.0f};
    glm::vec2 endPos = {0.0f, 0.0f};

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
    boxSelectShader.load(CURSOR_BOX_SELECT_VERT, CURSOR_BOX_SELECT_FRAG);
    boxSelectShader.use();
    float r = CURSOR_BOX_SELECT_COLOR.r;
    float g = CURSOR_BOX_SELECT_COLOR.g;
    float b = CURSOR_BOX_SELECT_COLOR.b;
    float a = CURSOR_BOX_SELECT_COLOR.a;
    boxSelectShader.setUniform4f("uColor", r, g, b, a); // TODO: pass vec4 directly into setUniform4f
}

void Cursor::_updateVertices() {
    float vertices[8] = {
        begPos.x, begPos.y,
        endPos.x, begPos.y,
        endPos.x, endPos.y,
        begPos.x, endPos.y
    };

    glBindBuffer(GL_ARRAY_BUFFER, boxSelectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void Cursor::update(Camera& camera, const Core::UserInput& input) {
    bool mouseL = (input.mouse.buttons & SDL_BUTTON_LMASK) != 0;

    // start selecting
    if (mouseL && !mouseLPrev) {
        isSelecting = true;
        begPos = camera.screenToWorld(
            input.mouse.x,
            input.mouse.y,
            input.window.width,
            input.window.height
        );
        endPos = begPos;
        _updateVertices();
    }

    // update selection box
    if (isSelecting) {
        endPos = camera.screenToWorld(
            input.mouse.x,
            input.mouse.y,
            input.window.width,
            input.window.height
        );
        _updateVertices();
    }

    // stop selecting
    if (!mouseL && mouseLPrev) {
        isSelecting = false;
    }

    mouseLPrev = mouseL;
}

void Cursor::render(Camera& camera) {
    if (!isSelecting) return;

    boxSelectShader.use();
    boxSelectShader.setUniformMatrix4fv("uVP", 1, camera.getViewProjMatrix());

    glLineWidth(2.0f);
    glBindVertexArray(boxSelectVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

} // namespace GFX