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

// shaders
constexpr char* CHUNK_BORDER_VERT    = "assets/shaders/chunk_border_vert.glsl";
constexpr char* CHUNK_BORDER_FRAG    = "assets/shaders/chunk_border_frag.glsl";
constexpr char* CHUNK_GRIDLINES_VERT = "assets/shaders/chunk_gridlines_vert.glsl";
constexpr char* CHUNK_GRIDLINES_FRAG = "assets/shaders/chunk_gridlines_frag.glsl";
constexpr char* CHUNK_SPRITES_VERT   = "assets/shaders/chunk_sprites_vert.glsl";
constexpr char* CHUNK_SPRITES_FRAG   = "assets/shaders/chunk_sprites_frag.glsl";

// textures
constexpr char* CHUNK_SPRITES_TEXT = "assets/images/chunk_sprites.png";

// constants
constexpr unsigned int CHUNK_TILE_COUNT_X = 16;
constexpr unsigned int CHUNK_TILE_COUNT_Y = 16;
constexpr unsigned int CHUNK_TILE_COUNT = CHUNK_TILE_COUNT_X * CHUNK_TILE_COUNT_Y;
constexpr glm::ivec2 CHUNK_TILE_SIZE = glm::ivec2(64, 32);
constexpr glm::ivec2 CHUNK_SIZE = glm::ivec2(64 * CHUNK_TILE_COUNT_X, 32 * CHUNK_TILE_COUNT_Y);

namespace GFX {

// TODO: should probably be renamed to isometric chunk renderer

class ChunkRenderer {
public:
    ChunkRenderer();
    ~ChunkRenderer();
    void render(Camera& camera);

private:
    void _setupOffsetInfo();
    void _setupBorderPipeline();
    void _setupGridlinesPipeline();
    void _setupSpritesPipeline();

    // chunk tile offsets information
    unsigned int offsetsVBO;
    glm::vec2 offsets[CHUNK_TILE_COUNT]; // todo should be static float?

    // chunk border information
    unsigned int borderVAO;
    unsigned int borderVBO;
    static float borderVertices[10];
    Shader borderShader;

    // chunk tile gridlines information
    unsigned int gridlinesVAO;
    unsigned int gridlinesVBO;
    static float gridlinesVertices[10];
    Shader gridlinesShader;

    // chunk tile sprites information
    unsigned int spritesVAO;
    unsigned int spritesVBO;
    unsigned int spritesEBO;
    static float spritesVertices[16];
    static unsigned int spritesIndices[6];
    Shader spritesShader;
    Texture spritesTexture;
};

float ChunkRenderer::borderVertices[10] = {
     0.0f*CHUNK_SIZE.x, 0.0f*CHUNK_SIZE.y, // iso chunk bottom
     0.5f*CHUNK_SIZE.x, 0.5f*CHUNK_SIZE.y, // iso chunk right
     0.0f*CHUNK_SIZE.x, 1.0f*CHUNK_SIZE.y, // iso chunk top
    -0.5f*CHUNK_SIZE.x, 0.5f*CHUNK_SIZE.y, // iso chunk left
     0.0f*CHUNK_SIZE.x, 0.0f*CHUNK_SIZE.y  // iso chunk bottom (close the line loop)
};

float ChunkRenderer::gridlinesVertices[10] = {
     0.0f*CHUNK_TILE_SIZE.x, 0.0f*CHUNK_TILE_SIZE.y, // iso tile bottom
     0.5f*CHUNK_TILE_SIZE.x, 0.5f*CHUNK_TILE_SIZE.y, // iso tile right
     0.0f*CHUNK_TILE_SIZE.x, 1.0f*CHUNK_TILE_SIZE.y, // iso tile top
    -0.5f*CHUNK_TILE_SIZE.x, 0.5f*CHUNK_TILE_SIZE.y, // iso tile left
     0.0f*CHUNK_TILE_SIZE.x, 0.0f*CHUNK_TILE_SIZE.y  // iso tile bottom (close the line loop)
};

float ChunkRenderer::spritesVertices[16] = {
    // positions                                       texcoords
    -0.5f*CHUNK_TILE_SIZE.x, 0.0f*CHUNK_TILE_SIZE.y,   0.0f, 0.0f, // quad bottom left
     0.5f*CHUNK_TILE_SIZE.x, 0.0f*CHUNK_TILE_SIZE.y,   1.0f, 0.0f, // quad bottom right
     0.5f*CHUNK_TILE_SIZE.x, 1.0f*CHUNK_TILE_SIZE.y,   1.0f, 1.0f, // quad top right
    -0.5f*CHUNK_TILE_SIZE.x, 1.0f*CHUNK_TILE_SIZE.y,   0.0f, 1.0f  // quad top left
};

unsigned int ChunkRenderer::spritesIndices[6] = {
    0, 1, 2, // first triangle of quad
    2, 3, 0  // second triangle of quad
};

ChunkRenderer::ChunkRenderer() {
    _setupOffsetInfo();
    _setupBorderPipeline();
    _setupGridlinesPipeline();
    _setupSpritesPipeline();
}

void ChunkRenderer::_setupOffsetInfo() {

    // order chunk tiles from back to front
    for (int y = 0; y < CHUNK_TILE_COUNT_Y; y++) {
        for (int x = 0; x < CHUNK_TILE_COUNT_X; x++) {
            int i = y * CHUNK_TILE_COUNT_X + x;
            int tx = CHUNK_TILE_COUNT_X - x - 1;
            int ty = CHUNK_TILE_COUNT_Y - y - 1;
            offsets[i] = tileToWorld(glm::ivec2(tx, ty), CHUNK_TILE_SIZE);
        }
    }

    // setup chunk tile offsets
    glGenBuffers(1, &offsetsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offsets), offsets, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::_setupBorderPipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &borderVAO);
    glGenBuffers(1, &borderVBO);

    // bind the vertex array object
    glBindVertexArray(borderVAO);

    // setup chunk tile gridline vertices
    glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(borderVertices), borderVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    borderShader.load(CHUNK_BORDER_VERT, CHUNK_BORDER_FRAG);
}

void ChunkRenderer::_setupGridlinesPipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &gridlinesVAO);
    glGenBuffers(1, &gridlinesVBO);

    // bind the vertex array object
    glBindVertexArray(gridlinesVAO);

    // setup chunk tile gridline vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridlinesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridlinesVertices), gridlinesVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup chunk tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, offsetsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridlinesShader.load(CHUNK_GRIDLINES_VERT, CHUNK_GRIDLINES_FRAG);
}

void ChunkRenderer::_setupSpritesPipeline() {

    // generate the opengl objects
    glGenVertexArrays(1, &spritesVAO);
    glGenBuffers(1, &spritesVBO);
    glGenBuffers(1, &spritesEBO);

    // bind the vertex array object
    glBindVertexArray(spritesVAO);

    // setup chunk tile sprite vertices
    glBindBuffer(GL_ARRAY_BUFFER, spritesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spritesVertices), spritesVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // setup chunk tile sprite indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spritesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spritesIndices), spritesIndices, GL_STATIC_DRAW);

    // setup chunk tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, offsetsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // unbind objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader and texture
    spritesShader.load(CHUNK_SPRITES_VERT, CHUNK_SPRITES_FRAG);
    spritesTexture.load(CHUNK_SPRITES_TEXT, true);
}

ChunkRenderer::~ChunkRenderer() {
    // delete offset objects
    if (offsetsVBO) glDeleteBuffers(1, &offsetsVBO);

    // delete border objects
    if (borderVAO) glDeleteVertexArrays(1, &borderVAO);
    if (borderVBO) glDeleteBuffers(1, &borderVBO);

    // delete gridline objects
    if (gridlinesVAO) glDeleteVertexArrays(1, &gridlinesVAO);
    if (gridlinesVBO) glDeleteBuffers(1, &gridlinesVBO);

    // delete sprite objects
    if (spritesVAO) glDeleteVertexArrays(1, &spritesVAO);
    if (spritesVBO) glDeleteBuffers(1, &spritesVBO);
    if (spritesEBO) glDeleteBuffers(1, &spritesEBO);
}

void ChunkRenderer::render(Camera& camera) {
    glm::mat4x4 vp =  camera.getViewProjMatrix();

    // int chunkX = 0.0f;
    // int chunkY = 0.0f;
    // float anchorX = (chunkX - chunkY) * CHUNK_SIZE * 0.50f;
    // float anchorY = (chunkX + chunkY) * CHUNK_SIZE * 0.25f;

    // glm::vec2 tilePos = worldToTile(camera.getPosition(), TILE_SIZE);
    // std::cout << tilePos.x << " " << tilePos.y << std::endl;

    std::vector<glm::vec2> origins = {
        tileToWorld(glm::ivec2(0, 0), CHUNK_SIZE),
        tileToWorld(glm::ivec2(1, 0), CHUNK_SIZE),
        tileToWorld(glm::ivec2(0, 1), CHUNK_SIZE),
        tileToWorld(glm::ivec2(1, 1), CHUNK_SIZE)
    };

    for (glm::vec2& origin : origins) {

        // render chunk sprites
        spritesShader.use();
        // spritesShader.setUniform2f("uOrigin", 0.0f, 0.0f);
        spritesShader.setUniform2f("uOrigin", origin.x, origin.y);
        spritesShader.setUniformMatrix4fv("uVP", 1, vp);
        spritesTexture.bind();
        glBindVertexArray(spritesVAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, CHUNK_TILE_COUNT);

        // render chunk gridlines
        gridlinesShader.use();
        // gridlinesShader.setUniform2f("uOrigin", 0.0f, 0.0f);
        gridlinesShader.setUniform2f("uOrigin", origin.x, origin.y);
        gridlinesShader.setUniform4f("uColor", 0.0f, 0.0f, 0.0f, 0.2f);
        gridlinesShader.setUniformMatrix4fv("uVP", 1, vp);
        glLineWidth(1.0f);
        glBindVertexArray(gridlinesVAO);
        glDrawArraysInstanced(GL_LINE_STRIP, 0, 5, CHUNK_TILE_COUNT);

        // render chunk border
        borderShader.use();
        // borderShader.setUniform2f("uOrigin", 0.0f, 0.0f);
        borderShader.setUniform2f("uOrigin", origin.x, origin.y);
        borderShader.setUniform4f("uColor", 0.0f, 0.0f, 1.0f, 1.0f);
        borderShader.setUniformMatrix4fv("uVP", 1, vp);
        glLineWidth(5.0f);
        glBindVertexArray(borderVAO);
        glDrawArraysInstanced(GL_LINE_STRIP, 0, 5, 1);
    }

    // unbind
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

} // namespace GFX