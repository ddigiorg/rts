#pragma once

#include "engine/gfx/types.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/gfx/texture.hpp"
#include "engine/utilities/assert.hpp"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include <vector>

#define CHUNK_LENGTH 16 // TODO: come up with a better name for this
#define TILE_SIZE 64.0f

namespace GFX {

class ChunkRenderer {
public:
    ChunkRenderer();
    ~ChunkRenderer();
    void update(const void* data);
    void render(Camera& camera);

private:
    static float vertices[];
    static unsigned int indices[];

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int positionVBO = 0;

    Shader shader;
    Texture texture;

    size_t capacity = CHUNK_LENGTH * CHUNK_LENGTH;

    // glm::vec2 positions[CHUNK_LENGTH][CHUNK_LENGTH];
};

float ChunkRenderer::vertices[] = {
    // positions   texcoords
    -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
     0.5f, 0.0f,   1.0f, 0.0f, // bottom right
     0.5f, 0.5f,   1.0f, 1.0f, // top right
    -0.5f, 0.5f,   0.0f, 1.0f  // top left
};

unsigned int ChunkRenderer::indices[] = {
    0, 1, 2, // first triangle
    2, 3, 0  // second triangle
};

ChunkRenderer::ChunkRenderer()
    : shader(TILE_VERT_FILEPATH, TILE_FRAG_FILEPATH),
      texture(TILE_TEXTURE_FILEPATH)
{
    // generate the opengl objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &positionVBO);

    // bind the vertex array object
    glBindVertexArray(VAO);

    // setup tile vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // setup tile indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // setup tile positions
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // unbind vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

ChunkRenderer::~ChunkRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (positionVBO) glDeleteBuffers(1, &positionVBO);
}

void ChunkRenderer::update(const void* data) {
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(glm::vec2), data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::render(Camera& camera) {
    // if (count == 0)
    //     return;

    glm::mat4x4 vp =  camera.getViewProjMatrix();

    texture.bind();

    shader.use();
    shader.setUniformMatrix4fv("uVP", 1, vp);
    shader.setUniform1f("uTileSize", TILE_SIZE);
    // shader.setUniform2f("uChunk");

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)capacity);
    glBindVertexArray(0);
}

} // namespace GFX