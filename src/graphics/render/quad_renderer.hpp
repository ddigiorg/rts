#pragma once

#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "utils/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Graphics {

// shader filepaths
constexpr const char* QUAD_VERT = "data/shaders/quad-instanced.vert";
constexpr const char* QUAD_FRAG = "data/shaders/quad-instanced.frag";

class QuadRenderer {
public:
    QuadRenderer();
    ~QuadRenderer();

    void updatePositionData(const size_t offset, const size_t num, const void* data);
    void updateScaleData(const size_t offset, const size_t num, const void* data);
    void updateColorData(const size_t offset, const size_t num, const void* data);

    void render(const Camera& camera);
    void renderOutline(const Camera& camera, const float lineWidth);

    void setCapacity(size_t newCapacity);
    void setCount(size_t newCount) { count = newCount; };
    size_t getCapacity() const { return capacity; };
    size_t getCount() const { return count; };

private:
    size_t capacity = 0; // max quads
    size_t count = 0; // active quads

    // dynamic data buffers
    unsigned int positionsVBO = 0; // (x, y, z)
    unsigned int scalesVBO = 0;    // (xw, yh)
    unsigned int colorsVBO = 0;    // (r, g, b, a)

    // quad pipeline
    unsigned int VAO = 0;
    unsigned int verticesVBO = 0;
    unsigned int indiciesEBO = 0;
    static float vertices[8];
    static unsigned int indices[6];
    Shader quadShader;
    Shader lineLoopShader;
};

// GLfloat QuadRenderer::vertices[8] = {
//     -0.5f, -0.5f, // bottom left
//      0.5f, -0.5f, // bottom right
//      0.5f,  0.5f, // top right
//     -0.5f,  0.5f, // top left
// };

float QuadRenderer::vertices[8] = {
    0.0f, 0.0f, // bottom left
    1.0f, 0.0f, // bottom right
    1.0f, 1.0f, // top right
    0.0f, 1.0f, // top left
};

unsigned int QuadRenderer::indices[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

QuadRenderer::QuadRenderer() {

    // generate the opengl objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &verticesVBO);
    glGenBuffers(1, &indiciesEBO);
    glGenBuffers(1, &positionsVBO);
    glGenBuffers(1, &scalesVBO);
    glGenBuffers(1, &colorsVBO);

    // initialize dynamic buffer sizes to 1 element
    setCapacity(1);

    // bind the vertex array object
    glBindVertexArray(VAO);

    // setup vertices
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // vertex position

    // setup indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // setup instance positions
    glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); // instance position
    glVertexAttribDivisor(1, 1);

    // setup instance scales
    glBindBuffer(GL_ARRAY_BUFFER, scalesVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2); // instance scale
    glVertexAttribDivisor(2, 1);

    // setup instance colors
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3); // instance color
    glVertexAttribDivisor(3, 1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup shaders
    quadShader.load(QUAD_VERT, QUAD_FRAG);
}

QuadRenderer::~QuadRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (verticesVBO) glDeleteBuffers(1, &verticesVBO);
    if (indiciesEBO) glDeleteBuffers(1, &indiciesEBO);
    if (positionsVBO) glDeleteBuffers(1, &positionsVBO);
    if (scalesVBO) glDeleteBuffers(1, &scalesVBO);
    if (colorsVBO) glDeleteBuffers(1, &colorsVBO);
}

void QuadRenderer::setCapacity(size_t newCapacity) {
    if (newCapacity == capacity) return;

    capacity = newCapacity;
    count = 0;

    // resize sprite positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize sprite scales buffer
    glBindBuffer(GL_ARRAY_BUFFER, scalesVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize sprite texcoords buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

void QuadRenderer::updatePositionData(const size_t offset, const size_t num, const void* data) {
    ASSERT(offset + num <= capacity, "Data range out of bounds.");
    ASSERT(num != 0, "Data is empty.");
    ASSERT(data != nullptr, "Data does not exist.");

    size_t elementSize = 3 * sizeof(float);
    size_t bufferOffset = offset * elementSize;
    size_t bufferSize = num * elementSize;
    glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadRenderer::updateScaleData(const size_t offset, const size_t num, const void* data) {
    ASSERT(offset + num <= capacity, "Data range out of bounds.");
    ASSERT(num != 0, "Data is empty.");
    ASSERT(data != nullptr, "Data does not exist.");

    size_t elementSize = 2 * sizeof(float);
    size_t bufferOffset = offset * elementSize;
    size_t bufferSize = num * elementSize;
    glBindBuffer(GL_ARRAY_BUFFER, scalesVBO);
    glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadRenderer::updateColorData(const size_t offset, const size_t num, const void* data) {
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

} // namespace Graphics