#pragma once

#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "utils/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Graphics {

// shader filepaths
constexpr const char* MARKERS_VERT = "assets/shaders/markers_vert.glsl";
constexpr const char* MARKERS_FRAG = "assets/shaders/markers_frag.glsl";

class MarkersRenderer {
public:
    MarkersRenderer();
    ~MarkersRenderer();

    void resize(size_t newCapacity);
    void resetCount() { count = 0; };
    void appendSubset(const size_t num, const void* transformsData);
    void render(const Camera& camera);

private:
    void _setupDataBuffers();
    void _setupPipeline();

    size_t count = 0;
    size_t capacity = 0;

    // dynamic data buffers
    unsigned int transformsVBO = 0;

    // quad pipeline
    unsigned int VAO = 0;
    unsigned int verticesVBO = 0;
    static float vertices[8];
    Shader shader;
};

// GLfloat MarkersRenderer::vertices[8] = {
//     -0.5f, -0.5f, // bottom left
//      0.5f, -0.5f, // bottom right
//      0.5f,  0.5f, // top right
//     -0.5f,  0.5f, // top left
// };

GLfloat MarkersRenderer::vertices[8] = {
    0.0f, 0.0f, // bottom left
    1.0f, 0.0f, // bottom right
    1.0f, 1.0f, // top right
    0.0f, 1.0f, // top left
};

MarkersRenderer::MarkersRenderer() {
    _setupDataBuffers();
    _setupPipeline();
}

void MarkersRenderer::_setupDataBuffers() {

    // generate opengl objects
    glGenBuffers(1, &transformsVBO);

    // initialize buffer sizes to 1 element
    resize(1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MarkersRenderer::_setupPipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &verticesVBO);

    // bind the vertex array object
    glBindVertexArray(VAO);

    // setup quad vertices
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup quad transforms (position)
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // setup quad transforms (scale)
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup shader
    shader.load(MARKERS_VERT, MARKERS_FRAG);
}

MarkersRenderer::~MarkersRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (verticesVBO) glDeleteBuffers(1, &verticesVBO);
    if (transformsVBO) glDeleteBuffers(1, &transformsVBO);
}

void MarkersRenderer::resize(size_t newCapacity) {
    if (newCapacity == capacity) return;

    capacity = newCapacity;
    count = 0;

    // resize quad transforms buffer
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

void MarkersRenderer::appendSubset(const size_t num, const void* transformsData) {
    ASSERT(num + count <= capacity, "Requested data insertion out of bounds.");

    size_t elementSize = 0;
    size_t bufferOffset = 0;
    size_t bufferSize = 0;

    // update quad transforms
    if (transformsData != nullptr) {
        elementSize = 4 * sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = num * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, transformsData);
    }

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    count += num;
}

void MarkersRenderer::render(const Camera& camera) {
    if (count == 0) return;

    const glm::mat4& vp =  camera.getViewProjMatrix();

    // // drop shadow
    // shader.use();
    // shader.setUniform2f("uOffset", 1.0f, -1.0f);
    // shader.setUniform4f("uColor", 1.0f, 0.0f, 0.0f, 1.0f);
    // shader.setUniformMatrix4fv("uVP", 1, vp);
    // glBindVertexArray(VAO);
    // glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, (GLsizei)count);

    // run pipeline
    shader.use();
    shader.setUniform2f("uOffset", 0.0f, 0.0f);
    shader.setUniform4f("uColor", 1.0f, 1.0f, 1.0f, 1.0f);
    shader.setUniformMatrix4fv("uVP", 1, vp);
    glLineWidth(2.0f);
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_LINE_LOOP, 0, 4, (GLsizei)count);

    // unbind
    glLineWidth(1.0f);
    glBindVertexArray(0);

}

} // namespace Graphics