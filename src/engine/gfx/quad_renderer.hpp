#pragma once

#include "engine/gfx/camera.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/utilities/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace GFX {

// shader filepaths
constexpr const char* QUAD_VERT = "assets/shaders/quad_vert.glsl";
constexpr const char* QUAD_FRAG = "assets/shaders/quad_frag.glsl";

class QuadRenderer {
public:
    QuadRenderer();
    ~QuadRenderer();

    void resize(size_t newCapacity);
    void resetCount() { count = 0; };

    void appendSubset(
        const size_t numQuads,
        const void* quadOffsetsData,
        const void* quadSizesData,
        const void* quadColorsData
    );

    void render(Camera& camera);

private:
    void _setupDataBuffers();
    void _setupQuadPipeline();

    size_t count = 0; // active quads
    size_t capacity = 0; // max quads

    // data buffers
    unsigned int quadOffsetsVBO = 0;
    unsigned int quadSizesVBO = 0;
    unsigned int quadColorsVBO = 0;

    // quad pipeline
    unsigned int quadsVAO = 0;
    unsigned int quadVerticesVBO = 0;
    unsigned int quadIndiciesEBO = 0;
    static float quadVertices[8];
    static unsigned int quadIndices[6];
    Shader quadShader;
};

GLfloat QuadRenderer::quadVertices[] = {
    -0.5f, -0.5f, // bottom left
     0.5f, -0.5f, // bottom right
     0.5f,  0.5f, // top right
    -0.5f,  0.5f, // top left
};

GLuint QuadRenderer::quadIndices[] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

QuadRenderer::QuadRenderer() {
    _setupDataBuffers();
    _setupQuadPipeline();
}

void QuadRenderer::_setupDataBuffers() {

    // generate opengl objects
    glGenBuffers(1, &quadOffsetsVBO);
    glGenBuffers(1, &quadSizesVBO);
    glGenBuffers(1, &quadColorsVBO);

    // initialize buffer sizes to 1 element
    resize(1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadRenderer::_setupQuadPipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &quadsVAO);
    glGenBuffers(1, &quadVerticesVBO);
    glGenBuffers(1, &quadIndiciesEBO);

    // bind the vertex array object
    glBindVertexArray(quadsVAO);

    // setup quad vertices
    glBindBuffer(GL_ARRAY_BUFFER, quadVerticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup quad indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIndiciesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // setup quad offsets
    glBindBuffer(GL_ARRAY_BUFFER, quadOffsetsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // setup quad sizes
    glBindBuffer(GL_ARRAY_BUFFER, quadSizesVBO);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // setup quad colors
    glBindBuffer(GL_ARRAY_BUFFER, quadColorsVBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup shader
    quadShader.load(QUAD_VERT, QUAD_FRAG);
}

QuadRenderer::~QuadRenderer() {
    if (quadsVAO) glDeleteVertexArrays(1, &quadsVAO);
    if (quadVerticesVBO) glDeleteBuffers(1, &quadVerticesVBO);
    if (quadIndiciesEBO) glDeleteBuffers(1, &quadIndiciesEBO);
    if (quadOffsetsVBO) glDeleteBuffers(1, &quadOffsetsVBO);
    if (quadSizesVBO) glDeleteBuffers(1, &quadSizesVBO);
    if (quadColorsVBO) glDeleteBuffers(1, &quadColorsVBO);
}

void QuadRenderer::resize(size_t newCapacity) {
    if (newCapacity == capacity) return;

    capacity = newCapacity;
    count = 0;

    // resize quad offsets buffer
    glBindBuffer(GL_ARRAY_BUFFER, quadOffsetsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize quad sizes buffer
    glBindBuffer(GL_ARRAY_BUFFER, quadSizesVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize quad colors buffer
    glBindBuffer(GL_ARRAY_BUFFER, quadColorsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

void QuadRenderer::appendSubset(
        const size_t numQuads,
        const void* quadOffsetsData,
        const void* quadSizesData,
        const void* quadColorsData
) {
    ASSERT(numQuads + count <= capacity, "Requested data insertion out of bounds.");

    size_t elementSize = 0;
    size_t bufferOffset = 0;
    size_t bufferSize = 0;

    // update quad offsets
    if (quadOffsetsData != nullptr) {
        elementSize = 2 * sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = numQuads * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, quadOffsetsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, quadOffsetsData);
    }

    // update quad sizes
    if (quadSizesData != nullptr) {
        elementSize = sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = numQuads * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, quadSizesVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, quadSizesData);
    }

    // update quad colors
    if (quadColorsData != nullptr) {
        elementSize = 3 * sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = numQuads * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, quadColorsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, quadColorsData);
    }

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    count += numQuads;
}

void QuadRenderer::render(Camera& camera) {
    if (count == 0) return;

    const glm::mat4 vp =  camera.getViewProjMatrix();

    // run quads pipeline
    quadShader.use();
    quadShader.setUniformMatrix4fv("uVP", 1, vp);
    glBindVertexArray(quadsVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

} // namespace GFX