#pragma once

#include "common/assets.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex_buffer.hpp"
#include "utils/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <cstdint>

// =============================================================================
// Definitions
// =============================================================================

const uint32_t QUAD_INDICES[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

const float QUAD_VERTICES[8] = {
    0.0f, 0.0f, // bottom left
    1.0f, 0.0f, // bottom right
    1.0f, 1.0f, // top right
    0.0f, 1.0f, // top left
};

// =============================================================================
// Quad Renderer
// =============================================================================

class QuadRenderer {
public:
    QuadRenderer();
    ~QuadRenderer();

    void update(size_t offset, size_t size, const void* boundsData, const void* colorsData);
    void render(const Camera& camera);
    void renderOutline(const Camera& camera, const float lineWidth = 1.0f);

    void reset() { instanceCount = 0; };
    size_t size() { return instanceCount; };

private:
    void _growBuffers(size_t newCapacity);
    void _prepareDraw(const Camera& camera);

    size_t instanceCapacity = 32;
    size_t instanceCount = 0;

    GLuint VAO = 0;
    GLuint quadEBO = 0;

    VertexBuffer quadVertices;
    VertexBuffer instanceBounds;
    VertexBuffer instanceColors;

    Shader shader;
};

QuadRenderer::QuadRenderer()
    : quadVertices  (0, 2, GL_FLOAT, 2 * sizeof(float), 0, GL_STATIC_DRAW),
      instanceBounds(1, 4, GL_FLOAT, 4 * sizeof(float), 1, GL_DYNAMIC_DRAW),
      instanceColors(2, 4, GL_FLOAT, 4 * sizeof(float), 1, GL_DYNAMIC_DRAW)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &quadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_INDICES), QUAD_INDICES, GL_STATIC_DRAW);

    quadVertices.allocate(4);
    instanceBounds.allocate(instanceCapacity);
    instanceColors.allocate(instanceCapacity);

    quadVertices.update(0, 4, QUAD_VERTICES);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Setup shader
    shader.load(Assets::SHADER_QUAD_INSTANCED_VERT, Assets::SHADER_QUAD_INSTANCED_FRAG);
}

QuadRenderer::~QuadRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (quadEBO) glDeleteBuffers(1, &quadEBO);
}

void QuadRenderer::update(size_t offset, size_t size, const void* boundsData, const void* colorsData) {
    ASSERT(size > 0, "Data is empty.");
    ASSERT(boundsData, "Bounds data is null.");
    ASSERT(colorsData, "Colors data is null.");
    ASSERT(offset <= instanceCount, "Offset creates a gap in instance data.");

    size_t newCount = offset + size;

    if (newCount > instanceCapacity)
        _growBuffers(static_cast<size_t>(newCount * 2.0f));

    instanceBounds.update(offset, size, boundsData);
    instanceColors.update(offset, size, colorsData);

    if (newCount > instanceCount)
        instanceCount = newCount;
}

void QuadRenderer::render(const Camera& camera) {
    if (instanceCount == 0) return;
    _prepareDraw(camera);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(instanceCount));
    glBindVertexArray(0);
}

void QuadRenderer::renderOutline(const Camera& camera, const float lineWidth) {
    if (instanceCount == 0) return;
    _prepareDraw(camera);
    glLineWidth(lineWidth);
    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, static_cast<GLsizei>(instanceCount));
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

void QuadRenderer::_growBuffers(size_t newCapacity) {
    glBindVertexArray(VAO);
    instanceBounds.resize(newCapacity);
    instanceColors.resize(newCapacity);
    glBindVertexArray(0);
    instanceCapacity = newCapacity;
}

void QuadRenderer::_prepareDraw(const Camera& camera) {
    shader.use();
    shader.setUniformMatrix4fv("uVP", 1, camera.getViewProjMatrix());
    glBindVertexArray(VAO);
}