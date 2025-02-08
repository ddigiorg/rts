#pragma once

#include "engine/gfx/types.hpp"
#include "engine/gfx/camera.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/gfx/texture.hpp"
#include "engine/utilities/assert.hpp"
#include "engine/utilities/isometric.hpp"

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include <vector>

constexpr unsigned int TILE_COUNT = 16;
constexpr glm::ivec2 TILE_SIZE = glm::ivec2(64, 32);
constexpr glm::ivec2 CHUNK_SIZE = glm::ivec2(64 * TILE_COUNT, 32 * TILE_COUNT);

namespace GFX {

class ChunkRenderer {
public:
    ChunkRenderer();
    ~ChunkRenderer();
    void render(Camera& camera);

private:
    void _setupOffsetInfo();
    void _setupSpritePipeline();
    void _setupGridlinePipeline();

    // tile offset information
    unsigned int offsetVBO;
    glm::vec2 offsets[TILE_COUNT][TILE_COUNT]; // todo should be static float?
    const int capacity = TILE_COUNT * TILE_COUNT;

    // tile sprite information
    unsigned int spriteVAO;
    unsigned int spriteVBO;
    unsigned int spriteEBO;
    static float spriteVertices[16];
    static unsigned int spriteIndices[6];
    Shader spriteShader;
    Texture spriteTexture;

    // tile gridline information
    unsigned int gridlineVAO;
    unsigned int gridlineVBO;
    static float gridlineVertices[10];
    Shader gridlineShader;
};

float ChunkRenderer::spriteVertices[16] = {
    // positions                          texcoords
    -0.5f*TILE_SIZE.x, 0.0f*TILE_SIZE.y,   0.0f, 0.0f, // quad bottom left
     0.5f*TILE_SIZE.x, 0.0f*TILE_SIZE.y,   1.0f, 0.0f, // quad bottom right
     0.5f*TILE_SIZE.x, 1.0f*TILE_SIZE.y,   1.0f, 1.0f, // quad top right
    -0.5f*TILE_SIZE.x, 1.0f*TILE_SIZE.y,   0.0f, 1.0f  // quad top left
};

unsigned int ChunkRenderer::spriteIndices[6] = {
    0, 1, 2, // first triangle of quad
    2, 3, 0  // second triangle of quad
};

float ChunkRenderer::gridlineVertices[10] = {
     0.0f*TILE_SIZE.x, 0.0f*TILE_SIZE.y, // iso tile bottom
     0.5f*TILE_SIZE.x, 0.5f*TILE_SIZE.y, // iso tile right
     0.0f*TILE_SIZE.x, 1.0f*TILE_SIZE.y, // iso tile top
    -0.5f*TILE_SIZE.x, 0.5f*TILE_SIZE.y, // iso tile left
     0.0f*TILE_SIZE.x, 0.0f*TILE_SIZE.y  // iso tile bottom (close the line loop)
};

ChunkRenderer::ChunkRenderer() {
    _setupOffsetInfo();
    _setupSpritePipeline();
    _setupGridlinePipeline();
}

void ChunkRenderer::_setupOffsetInfo() {

    for (int y = 0; y < TILE_COUNT; y++) {
        for (int x = 0; x < TILE_COUNT; x++) {
            int tx = TILE_COUNT - x - 1;
            int ty = TILE_COUNT - y - 1;
            offsets[y][x] = tileToWorld(glm::ivec2(tx, ty), TILE_SIZE);
        }
    }

    // setup tile offsets
    glGenBuffers(1, &offsetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offsets), offsets, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::_setupSpritePipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &spriteVAO);
    glGenBuffers(1, &spriteVBO);
    glGenBuffers(1, &spriteEBO);

    // bind the vertex array object
    glBindVertexArray(spriteVAO);

    // setup tile sprite vertices
    glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spriteVertices), spriteVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // setup tile sprite indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spriteEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spriteIndices), spriteIndices, GL_STATIC_DRAW);

    // setup tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader and texture
    spriteShader.load(TILE_SPRITE_VERT_FILEPATH, TILE_SPRITE_FRAG_FILEPATH);
    spriteTexture.load(TILE_TEXTURE_FILEPATH, true);
}

void ChunkRenderer::_setupGridlinePipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &gridlineVAO);
    glGenBuffers(1, &gridlineVBO);

    // bind the vertex array object
    glBindVertexArray(gridlineVAO);

    // setup tile gridline vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridlineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridlineVertices), gridlineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridlineShader.load(TILE_GRIDLINE_VERT_FILEPATH, TILE_GRIDLINE_FRAG_FILEPATH);
}

ChunkRenderer::~ChunkRenderer() {
    // delete offset objects
    if (offsetVBO) glDeleteBuffers(1, &offsetVBO);

    // delete sprite objects
    if (spriteVAO) glDeleteVertexArrays(1, &spriteVAO);
    if (spriteVBO) glDeleteBuffers(1, &spriteVBO);
    if (spriteEBO) glDeleteBuffers(1, &spriteEBO);

    // delete gridline objects
    if (gridlineVAO) glDeleteVertexArrays(1, &gridlineVAO);
    if (gridlineVBO) glDeleteBuffers(1, &gridlineVBO);
}

void ChunkRenderer::render(Camera& camera) {
    glm::mat4x4 vp =  camera.getViewProjMatrix();

    // int chunkX = 0.0f;
    // int chunkY = 0.0f;
    // float anchorX = (chunkX - chunkY) * CHUNK_SIZE * 0.50f;
    // float anchorY = (chunkX + chunkY) * CHUNK_SIZE * 0.25f;

    // glm::vec2 tilePos = worldToTile(camera.getPosition(), TILE_SIZE);
    // std::cout << tilePos.x << " " << tilePos.y << std::endl;

    // render tile sprites
    spriteShader.use();
    gridlineShader.setUniform2f("uOrigin", 0.0f, 0.0f);
    spriteShader.setUniformMatrix4fv("uVP", 1, vp);
    spriteTexture.bind();
    glBindVertexArray(spriteVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, capacity);

    // render tile gridlines
    gridlineShader.use();
    gridlineShader.setUniform2f("uOrigin", 0.0f, 0.0f);
    // gridlineShader.setUniform4f("uColor", 1.0f, 0.0f, 0.0f, 1.0f);
    gridlineShader.setUniform4f("uColor", 0.0f, 0.0f, 0.0f, 0.2f);
    gridlineShader.setUniformMatrix4fv("uVP", 1, vp);
    glBindVertexArray(gridlineVAO);
    glDrawArraysInstanced(GL_LINE_STRIP, 0, 5, capacity);

    // unbind
    glBindVertexArray(0);
}



} // namespace GFX