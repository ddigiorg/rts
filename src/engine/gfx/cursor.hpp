#pragma once

#include "engine/core/user_input.hpp"
#include "engine/gfx/types.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/shader.hpp"

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace GFX {

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
    void _updateVertices();

    bool isSelecting = false;
    bool mouseLPrev = false;

    glm::vec2 begPos = {0.0f, 0.0f};
    glm::vec2 endPos = {0.0f, 0.0f};

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Shader selectShader;
};

Cursor::Cursor()
    : selectShader(CURSOR_SELECT_VERT_FILEPATH, CURSOR_SELECT_FRAG_FILEPATH)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 2D position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    _updateVertices();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Cursor::~Cursor() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void Cursor::update(Camera& camera, const Core::UserInput& input) {
        bool mouseL = input.mouse.buttons == SDL_BUTTON_LMASK;

        // mouse 1 pressed
        if (mouseL && !mouseLPrev) {
            isSelecting = true;
            glm::vec2 worldPos = camera.screenToWorldCoords(
                input.mouse.x,
                input.mouse.y,
                800.0f,
                600.0f
            );
            begPos = worldPos;
            endPos = begPos;
            _updateVertices();
        }

        // mouse 1 held and moved
        if (isSelecting) {
            glm::vec2 worldPos = camera.screenToWorldCoords(
                input.mouse.x,
                input.mouse.y,
                800.0f,
                600.0f
            );
            endPos = worldPos;
            _updateVertices();
        }

        // mouse 1 released
        if (!mouseL && mouseLPrev) {
            isSelecting = false;
        }

        mouseLPrev = mouseL;
}

void Cursor::render(Camera& camera) {
    if (isSelecting) {
        glm::mat4x4 vp = camera.getViewProjMatrix();
        selectShader.use();
        selectShader.setUniformMatrix4fv("uVP", 1, vp);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
    }
}

void Cursor::_updateVertices() {
    float vertices[] = {
        begPos.x, begPos.y,
        endPos.x, begPos.y,
        endPos.x, endPos.y,
        begPos.x, endPos.y
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

} // namespace GFX