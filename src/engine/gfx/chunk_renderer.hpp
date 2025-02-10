#pragma once

#include "engine/gfx/camera.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/gfx/texture.hpp"
#include "engine/utilities/assert.hpp"
#include "engine/utilities/isometric.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <cstdint>

namespace GFX {

// shaders
constexpr char* CHUNK_BORDER_VERT    = "assets/shaders/chunk_border_vert.glsl";
constexpr char* CHUNK_BORDER_FRAG    = "assets/shaders/chunk_border_frag.glsl";
constexpr char* CHUNK_GRIDLINES_VERT = "assets/shaders/chunk_gridlines_vert.glsl";
constexpr char* CHUNK_GRIDLINES_FRAG = "assets/shaders/chunk_gridlines_frag.glsl";
constexpr char* CHUNK_SPRITES_VERT   = "assets/shaders/chunk_sprites_vert.glsl";
constexpr char* CHUNK_SPRITES_FRAG   = "assets/shaders/chunk_sprites_frag.glsl";

// textures
constexpr char* CHUNK_SPRITES_TEXT = "assets/images/chunk_sprites.png";

// tile constants
constexpr unsigned int TILE_SIZE_X = 64.0f;
constexpr unsigned int TILE_SIZE_Y = 32.0f;
constexpr unsigned int TILE_COUNT_X = 16;
constexpr unsigned int TILE_COUNT_Y = 16;
constexpr unsigned int TILE_COUNT = TILE_COUNT_X * TILE_COUNT_Y;
constexpr glm::vec2 TILE_SIZE = glm::vec2(TILE_SIZE_X, TILE_SIZE_Y);
constexpr glm::vec4 TILE_OUTLINE_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 0.2f);

// chunk constants
constexpr unsigned int CHUNK_SIZE_X = TILE_SIZE_X * TILE_COUNT_X;
constexpr unsigned int CHUNK_SIZE_Y = TILE_SIZE_Y * TILE_COUNT_Y;
constexpr glm::vec2 CHUNK_SIZE = glm::vec2(CHUNK_SIZE_X, CHUNK_SIZE_Y);
constexpr glm::vec4 CHUNK_OUTLINE_COLOR = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

// functions
inline glm::vec2 ChunkToWorld(const glm::vec2& chunk) {
    return IsoToWorld(chunk, CHUNK_SIZE);
}

inline glm::vec2 WorldToChunk(const glm::vec2& world) {
    return WorldToIso(world, CHUNK_SIZE);
}

inline glm::vec2 TileToWorld(const glm::vec2& tile) {
    return IsoToWorld(tile, TILE_SIZE);
}

inline glm::vec2 WorldToTile(const glm::vec2& world) {
    return WorldToIso(world, TILE_SIZE);
}

// TODO: should probably be renamed to isometric chunk renderer

class ChunkRenderer {
public:
    ChunkRenderer(const unsigned int numChunks);
    ~ChunkRenderer();

    void updateSubset(
        const unsigned int chunk,
        const void* chunkOffsetsData,
        const void* tileTypesData
    );

    void render(Camera& camera);

private:
    void _setupSharedBuffers();
    void _setupBorderPipeline();
    void _setupGridlinesPipeline();
    void _setupSpritesPipeline();

    // shared data buffers
    unsigned int chunkOffsetsVBO; // (dynamic) stores absolute positions of each chunk relative to (0, 0)
    unsigned int tileOffsetsVBO;  // (static) stores relative positions of tiles within a chunk
    unsigned int tileTypesVBO;    // (dynamic) stores tile types per chunk

    // chunk border pipeline
    unsigned int borderVAO = 0;
    unsigned int borderVBO = 0;
    static float borderVertices[10];
    Shader borderShader;

    // tile gridlines pipeline
    unsigned int gridlinesVAO = 0;
    unsigned int gridlinesVBO = 0;
    static float gridlinesVertices[10];
    Shader gridlinesShader;

    // tile sprites pipeline
    unsigned int spritesVAO = 0;
    unsigned int spritesVBO = 0;
    unsigned int spritesEBO = 0;
    static float spritesVertices[16];
    static unsigned int spritesIndices[6];
    Shader spritesShader;
    Texture spritesTexture;

    // buffer capacities
    unsigned int chunkCapacity = 0;
    unsigned int tileCapacity = 0;
};

float ChunkRenderer::borderVertices[10] = {
     0.0f*CHUNK_SIZE_X, 0.0f*CHUNK_SIZE_Y, // iso chunk bottom
     0.5f*CHUNK_SIZE_X, 0.5f*CHUNK_SIZE_Y, // iso chunk right
     0.0f*CHUNK_SIZE_X, 1.0f*CHUNK_SIZE_Y, // iso chunk top
    -0.5f*CHUNK_SIZE_X, 0.5f*CHUNK_SIZE_Y, // iso chunk left
     0.0f*CHUNK_SIZE_X, 0.0f*CHUNK_SIZE_Y  // iso chunk bottom (close the line loop)
};

float ChunkRenderer::gridlinesVertices[10] = {
     0.0f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y, // iso tile bottom
     0.5f*TILE_SIZE_X, 0.5f*TILE_SIZE_Y, // iso tile right
     0.0f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y, // iso tile top
    -0.5f*TILE_SIZE_X, 0.5f*TILE_SIZE_Y, // iso tile left
     0.0f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y  // iso tile bottom (close the line loop)
};

float ChunkRenderer::spritesVertices[16] = {
    // positions                           texcoords
    -0.5f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y,   0.0f, 0.0f, // quad bottom left
     0.5f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y,   1.0f, 0.0f, // quad bottom right
     0.5f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y,   1.0f, 1.0f, // quad top right
    -0.5f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y,   0.0f, 1.0f  // quad top left
};

unsigned int ChunkRenderer::spritesIndices[6] = {
    0, 1, 2, // first triangle of quad
    2, 3, 0  // second triangle of quad
};

ChunkRenderer::ChunkRenderer(const unsigned int numChunks) {
    chunkCapacity = numChunks;
    tileCapacity = TILE_COUNT * chunkCapacity;

    _setupSharedBuffers();
    _setupBorderPipeline();
    _setupGridlinesPipeline();
    _setupSpritesPipeline();
}

void ChunkRenderer::_setupSharedBuffers() {
    std::vector<glm::vec2> chunkOffsets;
    std::vector<glm::vec2> tileOffsets;
    std::vector<GLubyte> tileTypes;

    // setup chunk offsets
    for (unsigned int i = 0; i < chunkCapacity; i++) {
        glm::vec2 pos = ChunkToWorld(glm::vec2(i, 0));
        chunkOffsets.push_back(pos);
    }

    // setup tile offsets and types (NOTE: tile order is back to front)
    for (int i = 0; i < chunkOffsets.size(); i++) {
        for (int y = 0; y < TILE_COUNT_Y; y++) {
            for (int x = 0; x < TILE_COUNT_X; x++) {
                int tx = TILE_COUNT_X - x - 1;
                int ty = TILE_COUNT_Y - y - 1;
                glm::vec2 pos = TileToWorld(glm::vec2(tx, ty));
                tileOffsets.push_back(pos);
                tileTypes.push_back(0);
            }
        }
    }

    // generate opengl objects
    glGenBuffers(1, &chunkOffsetsVBO);
    glGenBuffers(1, &tileOffsetsVBO);
    glGenBuffers(1, &tileTypesVBO);

    // setup chunk offsets buffer
    glBindBuffer(GL_ARRAY_BUFFER, chunkOffsetsVBO);
    glBufferData(GL_ARRAY_BUFFER, chunkOffsets.size() * sizeof(glm::vec2), &chunkOffsets[0], GL_DYNAMIC_DRAW);

    // setup tile offsets buffer
    glBindBuffer(GL_ARRAY_BUFFER, tileOffsetsVBO);
    glBufferData(GL_ARRAY_BUFFER, tileOffsets.size() * sizeof(glm::vec2), &tileOffsets[0], GL_STATIC_DRAW);

    // setup tile types buffer
    glBindBuffer(GL_ARRAY_BUFFER, tileTypesVBO);
    glBufferData(GL_ARRAY_BUFFER, tileTypes.size() * sizeof(GLubyte), &tileTypes[0], GL_DYNAMIC_DRAW);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::_setupBorderPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &borderVAO);
    glGenBuffers(1, &borderVBO);

    // bind vertex array object
    glBindVertexArray(borderVAO);

    // setup chunk border vertices
    glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(borderVertices), borderVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup chunk offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkOffsetsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    borderShader.load(CHUNK_BORDER_VERT, CHUNK_BORDER_FRAG);
    borderShader.use();
    float r = CHUNK_OUTLINE_COLOR.r;
    float g = CHUNK_OUTLINE_COLOR.g;
    float b = CHUNK_OUTLINE_COLOR.b;
    float a = CHUNK_OUTLINE_COLOR.a;
    borderShader.setUniform4f("uColor", r, g, b, a);
}

void ChunkRenderer::_setupGridlinesPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &gridlinesVAO);
    glGenBuffers(1, &gridlinesVBO);

    // bind vertex array object
    glBindVertexArray(gridlinesVAO);

    // setup tile gridline vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridlinesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridlinesVertices), gridlinesVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup chunk tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, tileOffsetsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); // instance every tile

    // setup chunk offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkOffsetsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 256); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridlinesShader.load(CHUNK_GRIDLINES_VERT, CHUNK_GRIDLINES_FRAG);
    gridlinesShader.use();
    float r = TILE_OUTLINE_COLOR.r;
    float g = TILE_OUTLINE_COLOR.g;
    float b = TILE_OUTLINE_COLOR.b;
    float a = TILE_OUTLINE_COLOR.a;
    gridlinesShader.setUniform4f("uColor", r, g, b, a);
}

void ChunkRenderer::_setupSpritesPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &spritesVAO);
    glGenBuffers(1, &spritesVBO);
    glGenBuffers(1, &spritesEBO);

    // bind vertex array object
    glBindVertexArray(spritesVAO);

    // setup tile sprite vertices
    glBindBuffer(GL_ARRAY_BUFFER, spritesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spritesVertices), spritesVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // setup tile sprite indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spritesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spritesIndices), spritesIndices, GL_STATIC_DRAW);

    // setup tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, tileOffsetsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1); // instance every tile

    // setup tile types
    glBindBuffer(GL_ARRAY_BUFFER, tileTypesVBO);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(GLubyte), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1); // instance every tile

    // setup chunk offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkOffsetsVBO);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 256); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader and texture
    spritesTexture.load(CHUNK_SPRITES_TEXT, true);

    glm::vec2 sheetSize = spritesTexture.getSize();
    unsigned int sheetTilesPerRow = (unsigned int)(sheetSize.x / TILE_SIZE_X);

    spritesShader.load(CHUNK_SPRITES_VERT, CHUNK_SPRITES_FRAG);
    spritesShader.use();
    spritesShader.setUniform2f("uTileSize", TILE_SIZE_X, TILE_SIZE_Y);
    spritesShader.setUniform2f("uSheetSize", sheetSize.x, sheetSize.y);
    spritesShader.setUniform1ui("uTilesPerRow", sheetTilesPerRow);
}

ChunkRenderer::~ChunkRenderer() {
    // delete shared opengl objects
    if (chunkOffsetsVBO) glDeleteBuffers(1, &chunkOffsetsVBO);
    if (tileOffsetsVBO) glDeleteBuffers(1, &tileOffsetsVBO);
    if (tileTypesVBO) glDeleteBuffers(1, &tileTypesVBO);

    // delete chunk border opengl objects
    if (borderVAO) glDeleteVertexArrays(1, &borderVAO);
    if (borderVBO) glDeleteBuffers(1, &borderVBO);

    // delete tile gridline opengl objects
    if (gridlinesVAO) glDeleteVertexArrays(1, &gridlinesVAO);
    if (gridlinesVBO) glDeleteBuffers(1, &gridlinesVBO);

    // delete tile sprite opengl objects
    if (spritesVAO) glDeleteVertexArrays(1, &spritesVAO);
    if (spritesVBO) glDeleteBuffers(1, &spritesVBO);
    if (spritesEBO) glDeleteBuffers(1, &spritesEBO);
}

void ChunkRenderer::updateSubset(
        const unsigned int chunk,
        const void* chunkOffsetsData,
        const void* tileTypesData
) {
    ASSERT(chunk < chunkCapacity, "Requested chunk " << chunk << " out of bounds.");

    size_t bufferSize = 0;
    size_t bufferOffset = 0;

    // update chunk offset
    if (chunkOffsetsData != nullptr) {
        bufferSize = sizeof(glm::vec2);
        bufferOffset = chunk * bufferSize;
        glBindBuffer(GL_ARRAY_BUFFER, chunkOffsetsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, chunkOffsetsData);
    }

    // update chunk tile types
    if (tileTypesData != nullptr) {
        bufferSize = TILE_COUNT * sizeof(GLubyte);
        bufferOffset = chunk * bufferSize;
        glBindBuffer(GL_ARRAY_BUFFER, tileTypesVBO);
        glBufferSubData(GL_ARRAY_BUFFER, bufferOffset, bufferSize, tileTypesData);
    }

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::render(Camera& camera) {

    // get camera view projection matrix
    glm::mat4x4 vp =  camera.getViewProjMatrix();

    // render chunk sprites
    spritesShader.use();
    spritesShader.setUniformMatrix4fv("uVP", 1, vp);
    spritesTexture.bind();
    glBindVertexArray(spritesVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, tileCapacity);

    // render chunk gridlines
    gridlinesShader.use();
    gridlinesShader.setUniformMatrix4fv("uVP", 1, vp);
    glLineWidth(1.0f);
    glBindVertexArray(gridlinesVAO);
    glDrawArraysInstanced(GL_LINE_STRIP, 0, 5, tileCapacity);

    // render chunk border
    borderShader.use();
    borderShader.setUniformMatrix4fv("uVP", 1, vp);
    glLineWidth(5.0f);
    glBindVertexArray(borderVAO);
    glDrawArraysInstanced(GL_LINE_STRIP, 0, 5, chunkCapacity);

    // unbind
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

} // namespace GFX