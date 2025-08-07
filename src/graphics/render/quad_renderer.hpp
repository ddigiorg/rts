#pragma once

#include "config/assets.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "util/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <cstdint>

class QuadRenderer {
public:
    QuadRenderer();
    ~QuadRenderer();

    void updateBounds(const size_t offset, const size_t num, const void* data);
    void updateColors(const size_t offset, const size_t num, const void* data);

    void render(const Camera& camera);
    void renderOutline(const Camera& camera, const float lineWidth);

    void setCapacity(size_t newCapacity);
    void setCount(size_t newCount) { count = newCount; };
    size_t getCapacity() const { return capacity; };
    size_t getCount() const { return count; };

private:
    size_t capacity = 0; // max quads
    size_t count = 0; // active quads

    // dynamic data
    uint32_t boundsVBO = 0; // instance bound (minX, minY, maxX, maxY)
    uint32_t colorsVBO = 0; // instance color (r, g, b, a)

    // quad pipeline
    static uint32_t indices[6];
    static float vertices[8];
    uint32_t VAO = 0;
    uint32_t indiciesEBO = 0;
    uint32_t verticesVBO = 0;
    Shader quadShader;
    Shader lineLoopShader;
};

uint32_t QuadRenderer::indices[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

float QuadRenderer::vertices[8] = {
    0.0f, 0.0f, // bottom left
    1.0f, 0.0f, // bottom right
    1.0f, 1.0f, // top right
    0.0f, 1.0f, // top left
};

QuadRenderer::QuadRenderer() {

    // generate the opengl objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &verticesVBO);
    glGenBuffers(1, &indiciesEBO);
    glGenBuffers(1, &boundsVBO);
    glGenBuffers(1, &colorsVBO);

    // initialize dynamic buffer sizes to 1 element
    setCapacity(1);

    // bind the vertex array object
    glBindVertexArray(VAO);

    // setup indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // setup vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup instance bounds
    glBindBuffer(GL_ARRAY_BUFFER, boundsVBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // setup instance colors
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup shaders
    quadShader.load(Assets::Shaders::QUAD_INSTANCED_VERT, Assets::Shaders::QUAD_INSTANCED_FRAG);
}

QuadRenderer::~QuadRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (verticesVBO) glDeleteBuffers(1, &verticesVBO);
    if (indiciesEBO) glDeleteBuffers(1, &indiciesEBO);
    if (boundsVBO) glDeleteBuffers(1, &boundsVBO);
    if (colorsVBO) glDeleteBuffers(1, &colorsVBO);
}

void QuadRenderer::setCapacity(size_t newCapacity) {
    if (newCapacity == capacity) return;

    capacity = newCapacity;
    count = 0;

    // resize sprite positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, boundsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize sprite texcoords buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

void QuadRenderer::updateBounds(const size_t offset, const size_t num, const void* data) {
    ASSERT(offset + num <= capacity, "Data range out of bounds.");
    ASSERT(num != 0, "Data is empty.");
    ASSERT(data != nullptr, "Data does not exist.");

    size_t elementSize = 4 * sizeof(float);
    size_t bufferOffset = offset * elementSize;
    size_t bufferSize = num * elementSize;
    glBindBuffer(GL_ARRAY_BUFFER, boundsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadRenderer::updateColors(const size_t offset, const size_t num, const void* data) {
    ASSERT(offset + num <= capacity, "Data range out of bounds.");
    ASSERT(num != 0, "Data is empty.");
    ASSERT(data != nullptr, "Data does not exist.");

    size_t elementSize = 4 * sizeof(float);
    size_t bufferOffset = offset * elementSize;
    size_t bufferSize = num * elementSize;
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadRenderer::render(const Camera& camera) {
    if (count == 0) return;

    quadShader.use();
    quadShader.setUniformMatrix4fv("uVP", 1, camera.getViewProjMatrix());
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

void QuadRenderer::renderOutline(const Camera& camera, const float lineWidth) {
    if (count == 0) return;

    quadShader.use();
    quadShader.setUniformMatrix4fv("uVP", 1, camera.getViewProjMatrix());
    glLineWidth(lineWidth);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, (GLsizei)count);
    glLineWidth(1.0f);
    glBindVertexArray(0);
}
