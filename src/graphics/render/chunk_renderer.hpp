#pragma once

#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "utils/assert.hpp"
#include "utils/math.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace Graphics {

// shader filepaths
constexpr const char* CHUNK_TILES_VERT  = "assets/shaders/chunk_tiles_vert.glsl";
constexpr const char* CHUNK_TILES_FRAG  = "assets/shaders/chunk_tiles_frag.glsl";
constexpr const char* CHUNK_GRIDS_VERT  = "assets/shaders/chunk_grids_vert.glsl";
constexpr const char* CHUNK_GRIDS_FRAG  = "assets/shaders/chunk_grids_frag.glsl";
constexpr const char* CHUNK_BORDER_VERT = "assets/shaders/chunk_border_vert.glsl";
constexpr const char* CHUNK_BORDER_FRAG = "assets/shaders/chunk_border_frag.glsl";

// texture filepaths
constexpr const char* TILES_TEXT = "assets/images/tiles.png";

// tile constants
constexpr unsigned int TILE_SIZE_X = 32; // pixels
constexpr unsigned int TILE_SIZE_Y = 32; // pixels
constexpr glm::vec2 TILE_SIZE = glm::vec2(TILE_SIZE_X, TILE_SIZE_Y);
constexpr glm::vec4 TILE_GRID_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

// chunk constants
constexpr unsigned int CHUNK_WIDTH = 32; // tiles
constexpr unsigned int CHUNK_HEIGHT = 32; // tiles
constexpr unsigned int CHUNK_AREA = CHUNK_WIDTH * CHUNK_HEIGHT; // tiles
constexpr unsigned int CHUNK_SIZE_X = TILE_SIZE_X * CHUNK_WIDTH; // pixels
constexpr unsigned int CHUNK_SIZE_Y = TILE_SIZE_Y * CHUNK_HEIGHT; // pixels
constexpr glm::vec2 CHUNK_SIZE = glm::vec2(CHUNK_SIZE_X, CHUNK_SIZE_Y);
constexpr glm::vec4 CHUNK_BORDER_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

// functions
inline glm::vec2 ChunkToWorld(const glm::vec2& chunk){
    return orthoTileToWorld(chunk, CHUNK_SIZE);
}

inline glm::vec2 WorldToChunk(const glm::vec2& world){
    return orthoWorldToTile(world, CHUNK_SIZE);
}

inline glm::vec2 TileToWorld(const glm::vec2& tile){
    return orthoTileToWorld(tile, TILE_SIZE);
}

inline glm::vec2 WorldToTile(const glm::vec2& world){
    return orthoWorldToTile(world, TILE_SIZE);
}

// chunk renderer
class ChunkRenderer {
public:
    ChunkRenderer(const unsigned int numChunks);

    // TODO: maybe create should be like this
    // ChunkRenderer(
    //     const unsigned int numChunks,
    //     const glm::vec2& chunkArea,
    //     const glm::vec2& tileSize,
    // );

    ~ChunkRenderer();

    // TODO:
    // void resize(const unsigned int numChunks);

    void updateChunkPosition(const unsigned int c, const void* data);
    void updateChunkTiles(const unsigned int c, const void* data);
    void render(const Camera& camera);

private:
    void _setupDataBuffers();
    void _setupBorderPipeline();
    void _setupGridsPipeline();
    void _setupTilesPipeline();

    bool showGrids = true;
    bool showBorders = true;
    unsigned int chunkCapacity = 0;
    unsigned int tileCapacity = 0;

    // dynamic shared data buffers
    unsigned int chunkPositionsVBO; // absolute positions of chunk relative to (0, 0)
    unsigned int tileTypesVBO;      // tile types within a chunk

    // static shared data buffers
    unsigned int tilePositionsVBO;  // relative positions of tiles within a chunk

    // tiles pipeline
    unsigned int tilesVAO = 0;
    unsigned int tilesVBO = 0;
    unsigned int tilesEBO = 0;
    static float tileVertices[16];
    static unsigned int tileIndices[6];
    Shader tilesShader;
    Texture tilesTexture;

    // tile grids pipeline
    unsigned int gridsVAO = 0;
    unsigned int gridsVBO = 0;
    static float gridVertices[8];
    Shader gridsShader;

    // chunk borders pipeline
    unsigned int bordersVAO = 0;
    unsigned int bordersVBO = 0;
    static float borderVertices[8];
    Shader bordersShader;
};

float ChunkRenderer::borderVertices[8] = {
    0.0f*CHUNK_SIZE_X, 0.0f*CHUNK_SIZE_Y, // chunk bottom left
    1.0f*CHUNK_SIZE_X, 0.0f*CHUNK_SIZE_Y, // chunk bottom right
    1.0f*CHUNK_SIZE_X, 1.0f*CHUNK_SIZE_Y, // chunk top right
    0.0f*CHUNK_SIZE_X, 1.0f*CHUNK_SIZE_Y, // chunk top left
};

float ChunkRenderer::gridVertices[8] = {
    0.0f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y, // tile bottom left
    1.0f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y, // tile bottom right
    1.0f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y, // tile top right
    0.0f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y, // tile top left
};

float ChunkRenderer::tileVertices[16] = {
    // positions                          texcoords
    0.0f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y,   0.0f, 0.0f, // tile bottom left
    1.0f*TILE_SIZE_X, 0.0f*TILE_SIZE_Y,   1.0f, 0.0f, // tile bottom right
    1.0f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y,   1.0f, 1.0f, // tile top right
    0.0f*TILE_SIZE_X, 1.0f*TILE_SIZE_Y,   0.0f, 1.0f, // tile top left
};

unsigned int ChunkRenderer::tileIndices[6] = {
    0, 1, 2, // first triangle of tile
    2, 3, 0, // second triangle of tile
};

ChunkRenderer::ChunkRenderer(const unsigned int numChunks) {
    chunkCapacity = numChunks;
    tileCapacity = CHUNK_AREA * chunkCapacity;

    _setupDataBuffers();
    _setupBorderPipeline();
    _setupGridsPipeline();
    _setupTilesPipeline();
}

void ChunkRenderer::_setupDataBuffers() {
    std::vector<glm::vec2> chunkOffsets;
    std::vector<glm::vec2> tileOffsets;
    std::vector<GLubyte> tileTypes;

    // setup chunk positions
    for (unsigned int i = 0; i < chunkCapacity; i++) {
        chunkOffsets.push_back(glm::vec2(0.0f, 0.0f));
    }

    // setup tile positions and types
    // NOTE: tile render order is left to right from top to bottom
    for (int i = 0; i < chunkOffsets.size(); i++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                int ty = CHUNK_HEIGHT - y - 1;
                glm::vec2 pos = glm::vec2(x * TILE_SIZE_X, ty * TILE_SIZE_Y);
                tileOffsets.push_back(pos);
                tileTypes.push_back(0);
            }
        }
    }

    // generate opengl objects
    glGenBuffers(1, &chunkPositionsVBO);
    glGenBuffers(1, &tilePositionsVBO);
    glGenBuffers(1, &tileTypesVBO);

    // setup chunk positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glBufferData(GL_ARRAY_BUFFER, chunkOffsets.size() * sizeof(glm::vec2), &chunkOffsets[0], GL_DYNAMIC_DRAW);

    // setup tile positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, tilePositionsVBO);
    glBufferData(GL_ARRAY_BUFFER, tileOffsets.size() * sizeof(glm::vec2), &tileOffsets[0], GL_STATIC_DRAW);

    // setup tile types buffer
    glBindBuffer(GL_ARRAY_BUFFER, tileTypesVBO);
    glBufferData(GL_ARRAY_BUFFER, tileTypes.size() * sizeof(GLubyte), &tileTypes[0], GL_DYNAMIC_DRAW);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::_setupTilesPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &tilesVAO);
    glGenBuffers(1, &tilesVBO);
    glGenBuffers(1, &tilesEBO);

    // bind vertex array object
    glBindVertexArray(tilesVAO);

    // setup tile vertices
    glBindBuffer(GL_ARRAY_BUFFER, tilesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // tile vertex positions
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1); // tile vertex texcoords

    // setup tile indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tilesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tileIndices), tileIndices, GL_STATIC_DRAW);

    // setup tile world positions
    glBindBuffer(GL_ARRAY_BUFFER, tilePositionsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2); // tile world positions
    glVertexAttribDivisor(2, 1); // instance every tile

    // setup tile types
    glBindBuffer(GL_ARRAY_BUFFER, tileTypesVBO);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(GLubyte), (void*)0);
    glEnableVertexAttribArray(3); // tile types
    glVertexAttribDivisor(3, 1); // instance every tile

    // setup chunk offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(4); // chunk world positions
    glVertexAttribDivisor(4, CHUNK_AREA); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader and texture
    tilesTexture.load(TILES_TEXT, false);

    glm::vec2 sheetSize = tilesTexture.getSize();
    unsigned int sheetTilesPerRow = (unsigned int)(sheetSize.x / TILE_SIZE_X);

    tilesShader.load(CHUNK_TILES_VERT, CHUNK_TILES_FRAG);
    tilesShader.use();
    tilesShader.setUniform2f("uTileSize", TILE_SIZE_X, TILE_SIZE_Y);
    tilesShader.setUniform2f("uSheetSize", sheetSize.x, sheetSize.y);
    tilesShader.setUniform1ui("uTilesPerRow", sheetTilesPerRow);
}

void ChunkRenderer::_setupGridsPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &gridsVAO);
    glGenBuffers(1, &gridsVBO);

    // bind vertex array object
    glBindVertexArray(gridsVAO);

    // setup tile grid vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // tile vertex positions

    // setup tile grid positions
    glBindBuffer(GL_ARRAY_BUFFER, tilePositionsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1); // tile world positions
    glVertexAttribDivisor(1, 1); // instance every tile

    // setup chunk offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2); // chunk world positions
    glVertexAttribDivisor(2, CHUNK_AREA); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridsShader.load(CHUNK_GRIDS_VERT, CHUNK_GRIDS_FRAG);
    gridsShader.use();
    float r = TILE_GRID_COLOR.r;
    float g = TILE_GRID_COLOR.g;
    float b = TILE_GRID_COLOR.b;
    float a = TILE_GRID_COLOR.a;
    gridsShader.setUniform4f("uColor", r, g, b, a);
}

void ChunkRenderer::_setupBorderPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &bordersVAO);
    glGenBuffers(1, &bordersVBO);

    // bind vertex array object
    glBindVertexArray(bordersVAO);

    // setup chunk border vertices
    glBindBuffer(GL_ARRAY_BUFFER, bordersVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(borderVertices), borderVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // chunk vertex positions

    // setup chunk positions
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1); // chunk world positions
    glVertexAttribDivisor(1, 1); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    bordersShader.load(CHUNK_BORDER_VERT, CHUNK_BORDER_FRAG);
    bordersShader.use();
    float r = CHUNK_BORDER_COLOR.r;
    float g = CHUNK_BORDER_COLOR.g;
    float b = CHUNK_BORDER_COLOR.b;
    float a = CHUNK_BORDER_COLOR.a;
    bordersShader.setUniform4f("uColor", r, g, b, a); // TODO: pass vec4 into uniform
}

ChunkRenderer::~ChunkRenderer() {

    // delete shared opengl objects
    if (chunkPositionsVBO) glDeleteBuffers(1, &chunkPositionsVBO);
    if (tilePositionsVBO) glDeleteBuffers(1, &tilePositionsVBO);
    if (tileTypesVBO) glDeleteBuffers(1, &tileTypesVBO);

    // delete tile sprite opengl objects
    if (tilesVAO) glDeleteVertexArrays(1, &tilesVAO);
    if (tilesVBO) glDeleteBuffers(1, &tilesVBO);
    if (tilesEBO) glDeleteBuffers(1, &tilesEBO);

    // delete tile grids opengl objects
    if (gridsVAO) glDeleteVertexArrays(1, &gridsVAO);
    if (gridsVBO) glDeleteBuffers(1, &gridsVBO);

    // delete chunk border opengl objects
    if (bordersVAO) glDeleteVertexArrays(1, &bordersVAO);
    if (bordersVBO) glDeleteBuffers(1, &bordersVBO);
}

void ChunkRenderer::updateChunkPosition(const unsigned int c, const void* data) {
    ASSERT(c < chunkCapacity, "Requested chunk " << c << " out of bounds.");
    ASSERT(data != nullptr, "Data pointer must not be nullptr.");

    size_t size = sizeof(glm::vec2);
    size_t offset = c * size;
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::updateChunkTiles(const unsigned int c, const void* data) {
    ASSERT(c < chunkCapacity, "Requested chunk " << c << " out of bounds.");
    ASSERT(data != nullptr, "Data pointer must not be nullptr.");

    size_t size = CHUNK_AREA * sizeof(GLubyte);
    size_t offset = c * size;
    glBindBuffer(GL_ARRAY_BUFFER, tileTypesVBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkRenderer::render(const Camera& camera) {

    // get camera view projection matrix
    const glm::mat4& vp =  camera.getViewProjMatrix();

    // run chunk sprites pipeline
    tilesShader.use();
    tilesShader.setUniformMatrix4fv("uVP", 1, vp);
    tilesTexture.bind();
    glBindVertexArray(tilesVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, tileCapacity);

    // run chunk grids pipeline
    if (showGrids) {
        gridsShader.use();
        gridsShader.setUniformMatrix4fv("uVP", 1, vp);
        // glLineWidth(2.0f);
        glBindVertexArray(gridsVAO);
        glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, tileCapacity);
    }

    // run chunk border pipeline
    if (showBorders) {
        bordersShader.use();
        bordersShader.setUniformMatrix4fv("uVP", 1, vp);
        glLineWidth(3.0f);
        glBindVertexArray(bordersVAO);
        glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, chunkCapacity);
    }

    // unbind
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

} // namespace Graphics