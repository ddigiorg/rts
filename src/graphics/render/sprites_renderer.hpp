#pragma once

#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "utils/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Graphics {

// shader filepaths
constexpr const char* SPRITES_VERT = "assets/shaders/sprites_vert.glsl";
constexpr const char* SPRITES_FRAG = "assets/shaders/sprites_frag.glsl";

// texture filepaths
constexpr const char* SPRITES_TEXT = "assets/images/sprites.png";

class SpritesRenderer {
public:
    SpritesRenderer();
    ~SpritesRenderer();

    void resize(size_t newCapacity);
    void resetCount() { count = 0; };

    void appendSubset(
        const size_t num,
        const void* transformsData,
        const void* texcoordsData
    );

    void render(const Camera& camera);

private:
    void _setupDataBuffers();
    void _setupSpritesPipeline();

    size_t count = 0; // active sprites
    size_t capacity = 0; // max sprites

    // dynamic data buffers
    unsigned int transformsVBO = 0;
    unsigned int texcoordsVBO = 0;

    // sprite pipeline
    unsigned int spritesVAO = 0;
    unsigned int spriteVerticesVBO = 0;
    unsigned int spriteIndiciesEBO = 0;
    static float spriteVertices[16];
    static unsigned int spriteIndices[6];
    Shader spritesShader;
    Texture spritesTexture;
};

GLfloat SpritesRenderer::spriteVertices[16] = {
    // positions    // texcoords
    -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
     0.5f, -0.5f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f,   1.0f, 1.0f, // top right
    -0.5f,  0.5f,   0.0f, 1.0f, // top left
};

GLuint SpritesRenderer::spriteIndices[6] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

SpritesRenderer::SpritesRenderer() {
    _setupDataBuffers();
    _setupSpritesPipeline();
}

void SpritesRenderer::_setupDataBuffers() {

    // generate opengl objects
    glGenBuffers(1, &transformsVBO);
    glGenBuffers(1, &texcoordsVBO);

    // initialize buffer sizes to 1 element
    resize(1);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpritesRenderer::_setupSpritesPipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &spritesVAO);
    glGenBuffers(1, &spriteVerticesVBO);
    glGenBuffers(1, &spriteIndiciesEBO);

    // bind the vertex array object
    glBindVertexArray(spritesVAO);

    // setup sprite vertices
    glBindBuffer(GL_ARRAY_BUFFER, spriteVerticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // vertex positions
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1); // vertex texcoords

    // setup sprite indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteIndiciesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spriteIndices), spriteIndices, GL_STATIC_DRAW);

    // setup sprite transforms
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2); // sprite world positions (instanced)
    glVertexAttribDivisor(2, 1); // instance every sprite
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(3); // sprite world scales (instanced)
    glVertexAttribDivisor(3, 1); // instance every sprite

    // setup sprite texcoords
    glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(4); // sprite texture positions (instanced)
    glVertexAttribDivisor(4, 1); // instance every sprite
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(5); // sprite texture scales (instanced)
    glVertexAttribDivisor(5, 1); // instance every sprite

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // setup shader
    spritesShader.load(SPRITES_VERT, SPRITES_FRAG);
    spritesTexture.load(SPRITES_TEXT, true);
}

SpritesRenderer::~SpritesRenderer() {
    if (spritesVAO) glDeleteVertexArrays(1, &spritesVAO);
    if (spriteVerticesVBO) glDeleteBuffers(1, &spriteVerticesVBO);
    if (spriteIndiciesEBO) glDeleteBuffers(1, &spriteIndiciesEBO);
    if (transformsVBO) glDeleteBuffers(1, &transformsVBO);
    if (texcoordsVBO) glDeleteBuffers(1, &texcoordsVBO);
}

void SpritesRenderer::resize(size_t newCapacity) {
    if (newCapacity == capacity) return;

    capacity = newCapacity;
    count = 0;

    // resize sprite transforms buffer
    glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // resize sprite texcoords buffer
    glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
}

void SpritesRenderer::appendSubset(
        const size_t num,
        const void* transformData,
        const void* texcoordsData
) {
    ASSERT(num + count <= capacity, "Requested data insertion out of bounds.");

    size_t elementSize = 0;
    size_t bufferOffset = 0;
    size_t bufferSize = 0;

    // update sprite transforms
    if (transformData != nullptr) {
        elementSize = 4 * sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = num * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, transformsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, transformData);
    }

    // update quad colors
    if (texcoordsData != nullptr) {
        elementSize = 4 * sizeof(float);
        bufferOffset = count * elementSize;
        bufferSize = num * elementSize;
        glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, texcoordsData);
    }

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    count += num;
}

void SpritesRenderer::render(const Camera& camera) {
    if (count == 0) return;

    const glm::mat4& vp =  camera.getViewProjMatrix();
    const glm::vec3& cameraPos = camera.getPosition();
    float halfHeight = camera.getSize().y * 0.5f;    
    float yMin = cameraPos.y - halfHeight - 64.0f; // TODO: 64.0f should be something like 2 * SPRITE_HEIGHT
    float yMax = cameraPos.y + halfHeight + 64.0f;

    // run sprites pipeline
    spritesShader.use();
    spritesShader.setUniform2f("uYBounds", yMin, yMax);
    spritesShader.setUniformMatrix4fv("uVP", 1, vp);
    spritesTexture.bind();
    glBindVertexArray(spritesVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)count);
    // glMultiDrawElementsIndirect // TODO: look into?
    glBindVertexArray(0);
}

} // namespace Graphics