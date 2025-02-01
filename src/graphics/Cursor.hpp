// Cursor.hpp
#pragma once

#include "Types.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Shader.hpp"

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace GFX {

class Cursor {
public:
    Cursor();
    ~Cursor();
    void handleEvents(const Core::EventState events);
    void startSelection(float x, float y);
    void updateSelection(float x, float y);
    void endSelection();
    void render(Camera& camera);

    bool getIsSelecting() const { return isSelecting; };
    glm::vec2 getBegPos() const { return begPos; };
    glm::vec2 getEndPos() const { return endPos; };

private:
    void _updateVertices();

    bool isSelecting = false;
    // glm::vec2 posScreen = {0.0f, 0.0f};
    // glm::vec2 posWorld = {0.0f, 0.0f};

    glm::vec2 begPos = {0.0f, 0.0f};
    glm::vec2 endPos = {0.0f, 0.0f};

    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Shader selectShader;
};

Cursor::Cursor()
    : selectShader(CURSOR_SELECT_VERT_FILEPATH, CURSOR_SELECT_FRAG_FILEPATH) {

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

void Cursor::handleEvents(const Core::EventState events) {

        if (events.mouse.leftPressed) {
            startSelection(events.mouse.posWorldX, events.mouse.posWorldY);
        }

        if (events.mouse.moved) {
            updateSelection(events.mouse.posWorldX, events.mouse.posWorldY);
        }

        if (events.mouse.leftReleased) {
            endSelection();
        }
}

void Cursor::startSelection(float x, float y) {
    if (!isSelecting) {
        isSelecting = true;
        begPos = {x, y};
        endPos = begPos;
        _updateVertices();
    }
}

void Cursor::updateSelection(float x, float y) {
    if (isSelecting) {
        endPos = {x, y};
        _updateVertices();
    }
}

void Cursor::endSelection() {
    isSelecting = false;
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