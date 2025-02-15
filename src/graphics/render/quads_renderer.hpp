#pragma once

#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "utils/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Graphics {

// shader filepaths
constexpr const char* QUADS_VERT = "assets/shaders/quads_vert.glsl";
constexpr const char* QUADS_FRAG = "assets/shaders/quads_frag.glsl";

class QuadsRenderer {
public:
    QuadsRenderer();
    ~QuadsRenderer();

    void resize(size_t newCapacity);
    void resetCount() { count = 0; };

    void appendSubset(
        const size_t num,
        const void* transformsData,
        const void* colorsData
    );

    void render(const Camera& camera);

private:
    void _setupDataBuffers();
    void _setupQuadPipeline();

    size_t count = 0; // active quads
    size_t capacity = 0; // max quads

    // dynamic data buffers
    unsigned int transformsVBO = 0;
    unsigned int colorsVBO = 0;

    // quad pipeline
    unsigned int quadsVAO = 0;
    unsigned int quadVerticesVBO = 0;
    unsigned int quadIndiciesEBO = 0;
    static float quadVertices[8];
    static unsigned int quadIndices[6];
    Shader quadShader;
};

GLfloat QuadsRenderer::quadVertices[8] = {
    -0.5f, -0.5f, // bottom left
     0.5f, -0.5f, // bottom right
     0.5f,  0.5f, // top right
    -0.5f,  0.5f, // top left
};

GLuint QuadsRenderer::quadIndices[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

QuadsRenderer::QuadsRenderer() {
    _setupDataBuffers();
    _setupQuadPipeline();
}

void QuadsRenderer::_setupDataBuffers() {

    // generate opengl objects
    glGenBuffers(1, &transformsVBO);
    glGenBuffers(1, &colorsVBO);

    // initialize buffer sizes to 1 element
    resize(1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadsRenderer::_setupQuadPipeline() {

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

    // setup quad transforms (position)
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // setup quad transforms (size)
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // setup quad colors
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup shader
    quadShader.load(QUADS_VERT, QUADS_FRAG);
}

QuadsRenderer::~QuadsRenderer() {
    if (quadsVAO) glDeleteVertexArrays(1, &quadsVAO);
    if (quadVerticesVBO) glDeleteBuffers(1, &quadVerticesVBO);
    if (quadIndiciesEBO) glDeleteBuffers(1, &quadIndiciesEBO);
    if (transformsVBO) glDeleteBuffers(1, &transformsVBO);
    if (colorsVBO) glDeleteBuffers(1, &colorsVBO);
}

void QuadsRenderer::resize(size_t newCapacity) {
    if (newCapacity == capacity) return;

    capacity = newCapacity;
    count = 0;

    // resize quad transforms buffer
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize quad colors buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

void QuadsRenderer::appendSubset(
        const size_t num,
        const void* transformsData,
        const void* colorsData
) {
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

    // update quad colors
    if (colorsData != nullptr) {
        elementSize = 3 * sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = num * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, colorsData);
    }

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    count += num;
}

void QuadsRenderer::render(const Camera& camera) {
    if (count == 0) return;

    const glm::mat4& vp =  camera.getViewProjMatrix();

    // run quads pipeline
    quadShader.use();
    quadShader.setUniformMatrix4fv("uVP", 1, vp);
    glBindVertexArray(quadsVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

} // namespace Graphics