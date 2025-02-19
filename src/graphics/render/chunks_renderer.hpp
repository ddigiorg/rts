#pragma once

#include "core/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "utils/assert.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace Graphics {

// shader filepaths
constexpr const char* CHUNK_SPRITES_VERT = "assets/shaders/chunk_sprites_vert.glsl";
constexpr const char* CHUNK_SPRITES_FRAG = "assets/shaders/chunk_sprites_frag.glsl";
constexpr const char* CHUNK_GRIDS_VERT = "assets/shaders/chunk_grids_vert.glsl";
constexpr const char* CHUNK_GRIDS_FRAG = "assets/shaders/chunk_grids_frag.glsl";

// texture filepaths
constexpr const char* TILES_TEXT = "assets/images/tiles.png";

// grid color constants
constexpr glm::vec4 GRID_TILE_COLOR   = glm::vec4(0.0f, 0.0f, 0.0f, 0.2f);
constexpr glm::vec4 GRID_SECTOR_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
constexpr glm::vec4 GRID_CHUNK_COLOR  = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
constexpr float GRID_TILE_LINEWIDTH   = 1.0f;
constexpr float GRID_SECTOR_LINEWIDTH = 2.0f;
constexpr float GRID_CHUNK_LINEWIDTH  = 5.0f;

// chunks renderer
class ChunksRenderer {
public:
    ChunksRenderer(const unsigned int numChunks);
    ~ChunksRenderer();

    // TODO:
    // void resize(const unsigned int numChunks);

    void updateChunkPosition(const unsigned int c, const void* data);
    void updateChunkTiles(const unsigned int c, const void* data);
    void render(const Camera& camera);

private:
    void _setupDataBuffers();
    void _setupSpritesPipeline();
    void _setupGridTilesPipeline();
    void _setupGridSectorsPipeline();
    void _setupGridChunksPipeline();

    // variables
    bool showGridTiles = true;
    bool showGridSectors = true;
    bool showGridChunks = true;
    unsigned int tileCapacity = 0;
    unsigned int sectorCapacity = 0;
    unsigned int chunkCapacity = 0;

    // static data
    static float zeroVertices[8];
    static float gridVertices[8];
    static float tileVertices[16];
    static unsigned int tileIndices[6];

    // shared dynamic data buffers
    unsigned int chunkPositionsVBO = 0; // absolute positions of chunk relative to (0, 0)
    unsigned int chunkTileTypesVBO = 0; // tile types within a chunk

    // shared static data buffers
    unsigned int zeroVBO = 0;
    unsigned int tilePositionsVBO = 0;   // relative positions of tiles within a chunk
    unsigned int sectorPositionsVBO = 0; // relative positions of sectors within a chunk

    // sprites pipeline
    unsigned int spritesVAO = 0;
    unsigned int spritesVBO = 0;
    unsigned int spritesEBO = 0;
    Shader spritesShader;
    Texture spritesTexture;

    // grid tiles pipeline
    unsigned int gridTilesVAO = 0;
    unsigned int gridTilesVBO = 0;
    Shader gridTilesShader;

    // grid sectors pipeline
    unsigned int gridSectorsVAO = 0;
    unsigned int gridSectorsVBO = 0;
    Shader gridSectorsShader;

    // grid chunks pipeline
    unsigned int gridChunksVAO = 0;
    unsigned int gridChunksVBO = 0;
    Shader gridChunksShader;

};

float ChunksRenderer::zeroVertices[8] = {
    0.0f, 0.0f, // bottom left
    0.0f, 0.0f, // bottom right
    0.0f, 0.0f, // top right
    0.0f, 0.0f, // top left
};

float ChunksRenderer::gridVertices[8] = {
    0.0f, 0.0f, // bottom left
    1.0f, 0.0f, // bottom right
    1.0f, 1.0f, // top right
    0.0f, 1.0f, // top left
};

float ChunksRenderer::tileVertices[16] = {
    // positions  texcoords
    0.0f, 0.0f,   0.0f, 0.0f, // bottom left
    1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    1.0f, 1.0f,   1.0f, 1.0f, // top right
    0.0f, 1.0f,   0.0f, 1.0f, // top left
};

unsigned int ChunksRenderer::tileIndices[6] = {
    0, 1, 2, // first triangle of tile
    2, 3, 0, // second triangle of tile
};

ChunksRenderer::ChunksRenderer(const unsigned int numChunks) {
    chunkCapacity = numChunks;
    sectorCapacity = CHUNK_SECTOR_COUNT * chunkCapacity;
    tileCapacity = CHUNK_TILE_COUNT * chunkCapacity;

    _setupDataBuffers();
    _setupSpritesPipeline();
    _setupGridTilesPipeline();
    _setupGridSectorsPipeline();
    _setupGridChunksPipeline();
}

void ChunksRenderer::_setupDataBuffers() {
    std::vector<glm::vec2> chunkPositions;
    std::vector<glm::vec2> sectorPositions;
    std::vector<glm::vec2> tilePositions;
    std::vector<GLubyte> tileTypes;

    // setup chunk positions
    for (unsigned int i = 0; i < chunkCapacity; i++) {
        int x = i % CHUNK_RENDER_COUNT_X;
        int y = i / CHUNK_RENDER_COUNT_X;
        chunkPositions.push_back(glm::vec2(x, y));
    }

    // setup sector positions
    // NOTE: sector render order is left to right from top to bottom
    for (int i = 0; i < chunkPositions.size(); i++) {
        for (int y = 0; y < CHUNK_SECTOR_COUNT_Y; y++) {
            for (int x = 0; x < CHUNK_SECTOR_COUNT_X; x++) {
                int sy = CHUNK_SECTOR_COUNT_Y - y - 1;
                sectorPositions.push_back(glm::vec2(x * SECTOR_SIZE_X, sy * SECTOR_SIZE_Y));
            }
        }
    }

    // setup tile positions and types
    // NOTE: tile render order is left to right from top to bottom
    for (int i = 0; i < chunkPositions.size(); i++) {
        for (int y = 0; y < CHUNK_TILE_COUNT_Y; y++) {
            for (int x = 0; x < CHUNK_TILE_COUNT_X; x++) {
                int ty = CHUNK_TILE_COUNT_Y - y - 1;
                tilePositions.push_back(glm::vec2(x * TILE_SIZE_X, ty * TILE_SIZE_Y));
                tileTypes.push_back(0);
            }
        }
    }

    // generate opengl objects
    glGenBuffers(1, &zeroVBO);
    glGenBuffers(1, &chunkPositionsVBO);
    glGenBuffers(1, &sectorPositionsVBO);
    glGenBuffers(1, &tilePositionsVBO);
    glGenBuffers(1, &chunkTileTypesVBO);

    // setup zero buffer
    glBindBuffer(GL_ARRAY_BUFFER, zeroVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(zeroVertices), zeroVertices, GL_STATIC_DRAW);

    // setup chunk positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glBufferData(GL_ARRAY_BUFFER, chunkPositions.size() * sizeof(glm::vec2), &chunkPositions[0], GL_DYNAMIC_DRAW);

    // setup sector positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, sectorPositionsVBO);
    glBufferData(GL_ARRAY_BUFFER, sectorPositions.size() * sizeof(glm::vec2), &sectorPositions[0], GL_DYNAMIC_DRAW);

    // setup tile positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, tilePositionsVBO);
    glBufferData(GL_ARRAY_BUFFER, tilePositions.size() * sizeof(glm::vec2), &tilePositions[0], GL_STATIC_DRAW);

    // setup tile types buffer
    glBindBuffer(GL_ARRAY_BUFFER, chunkTileTypesVBO);
    glBufferData(GL_ARRAY_BUFFER, tileTypes.size() * sizeof(GLubyte), &tileTypes[0], GL_DYNAMIC_DRAW);

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunksRenderer::_setupSpritesPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &spritesVAO);
    glGenBuffers(1, &spritesVBO);
    glGenBuffers(1, &spritesEBO);

    // bind vertex array object
    glBindVertexArray(spritesVAO);

    // setup sprite vertices
    glBindBuffer(GL_ARRAY_BUFFER, spritesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tileVertices), tileVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // sprite vertex positions
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1); // sprite vertex texcoords

    // setup sprite indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, spritesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tileIndices), tileIndices, GL_STATIC_DRAW);

    // setup sprite positions
    glBindBuffer(GL_ARRAY_BUFFER, tilePositionsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2); // sprite world positions
    glVertexAttribDivisor(2, 1); // instance every tile

    // setup sprite tile types
    glBindBuffer(GL_ARRAY_BUFFER, chunkTileTypesVBO);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(GLubyte), (void*)0);
    glEnableVertexAttribArray(3); // sprite tile types
    glVertexAttribDivisor(3, 1); // instance every tile

    // setup chunk offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(4); // chunk world positions
    glVertexAttribDivisor(4, CHUNK_TILE_COUNT); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader and texture
    spritesTexture.load(TILES_TEXT, false);

    glm::vec2 sheetSize = spritesTexture.getSize();
    unsigned int sheetTilesPerRow = (unsigned int)(sheetSize.x / TILE_SIZE_X);

    spritesShader.load(CHUNK_SPRITES_VERT, CHUNK_SPRITES_FRAG);
    spritesShader.use();
    spritesShader.setUniform2f("uVertexSize", TILE_SIZE_X, TILE_SIZE_Y);
    spritesShader.setUniform2f("uSheetSize", sheetSize.x, sheetSize.y);
    spritesShader.setUniform1ui("uTilesPerRow", sheetTilesPerRow);
}

void ChunksRenderer::_setupGridTilesPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &gridTilesVAO);
    glGenBuffers(1, &gridTilesVBO);

    // bind vertex array object
    glBindVertexArray(gridTilesVAO);

    // setup grid tile vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridTilesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // grid vertex positions

    // setup grid tile positions
    glBindBuffer(GL_ARRAY_BUFFER, tilePositionsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1); // grid world positions
    glVertexAttribDivisor(1, 1); // instance every tile

    // setup grid tile offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2); // grid world offsets
    glVertexAttribDivisor(2, CHUNK_TILE_COUNT); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridTilesShader.load(CHUNK_GRIDS_VERT, CHUNK_GRIDS_FRAG);
    gridTilesShader.use();
    gridTilesShader.setUniform2f("uVertexSize", TILE_SIZE_X, TILE_SIZE_Y);
    float r = GRID_TILE_COLOR.r;
    float g = GRID_TILE_COLOR.g;
    float b = GRID_TILE_COLOR.b;
    float a = GRID_TILE_COLOR.a;
    gridTilesShader.setUniform4f("uColor", r, g, b, a);  // TODO: pass vec4 into uniform
}

void ChunksRenderer::_setupGridSectorsPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &gridSectorsVAO);
    glGenBuffers(1, &gridSectorsVBO);

    // bind vertex array object
    glBindVertexArray(gridSectorsVAO);

    // setup grid sector vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridSectorsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // grid vertex positions

    // setup grid sector positions
    glBindBuffer(GL_ARRAY_BUFFER, sectorPositionsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1); // grid world positions
    glVertexAttribDivisor(1, 1); // instance every sector

    // setup grid sector offsets
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2); // grid world offsets
    glVertexAttribDivisor(2, CHUNK_SECTOR_COUNT); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridSectorsShader.load(CHUNK_GRIDS_VERT, CHUNK_GRIDS_FRAG);
    gridSectorsShader.use();
    gridSectorsShader.setUniform2f("uVertexSize", SECTOR_SIZE_X, SECTOR_SIZE_Y);
    float r = GRID_SECTOR_COLOR.r;
    float g = GRID_SECTOR_COLOR.g;
    float b = GRID_SECTOR_COLOR.b;
    float a = GRID_SECTOR_COLOR.a;
    gridSectorsShader.setUniform4f("uColor", r, g, b, a);  // TODO: pass vec4 into uniform
}

void ChunksRenderer::_setupGridChunksPipeline() {

    // generate opengl objects
    glGenVertexArrays(1, &gridChunksVAO);
    glGenBuffers(1, &gridChunksVBO);

    // bind vertex array object
    glBindVertexArray(gridChunksVAO);

    // setup grid chunk vertices
    glBindBuffer(GL_ARRAY_BUFFER, gridChunksVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // grid vertex positions

    // setup grid chunk positions (zeroed)
    glBindBuffer(GL_ARRAY_BUFFER, zeroVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1); // zero positions
    glVertexAttribDivisor(1, 1); // instance every chunk

    // setup grid chunk positions
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2); // chunk world positions
    glVertexAttribDivisor(2, 1); // instance every chunk

    // unbind opengl objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load shader
    gridChunksShader.load(CHUNK_GRIDS_VERT, CHUNK_GRIDS_FRAG);
    gridChunksShader.use();
    gridChunksShader.setUniform2f("uVertexSize", CHUNK_SIZE_X, CHUNK_SIZE_Y);
    float r = GRID_CHUNK_COLOR.r;
    float g = GRID_CHUNK_COLOR.g;
    float b = GRID_CHUNK_COLOR.b;
    float a = GRID_CHUNK_COLOR.a;
    gridChunksShader.setUniform4f("uColor", r, g, b, a); // TODO: pass vec4 into uniform
}

ChunksRenderer::~ChunksRenderer() {

    // delete shared opengl objects
    if (zeroVBO) glDeleteBuffers(1, &zeroVBO);
    if (chunkPositionsVBO) glDeleteBuffers(1, &chunkPositionsVBO);
    if (tilePositionsVBO) glDeleteBuffers(1, &tilePositionsVBO);
    if (sectorPositionsVBO) glDeleteBuffers(1, &tilePositionsVBO);
    if (chunkTileTypesVBO) glDeleteBuffers(1, &chunkTileTypesVBO);

    // delete sprites opengl objects
    if (spritesVAO) glDeleteVertexArrays(1, &spritesVAO);
    if (spritesVBO) glDeleteBuffers(1, &spritesVBO);
    if (spritesEBO) glDeleteBuffers(1, &spritesEBO);

    // delete grid tiles opengl objects
    if (gridTilesVAO) glDeleteVertexArrays(1, &gridTilesVAO);
    if (gridTilesVBO) glDeleteBuffers(1, &gridTilesVBO);

    // delete grid sectors opengl objects
    if (gridSectorsVAO) glDeleteVertexArrays(1, &gridSectorsVAO);
    if (gridSectorsVBO) glDeleteBuffers(1, &gridSectorsVBO);

    // delete grid chunks opengl objects
    if (gridChunksVAO) glDeleteVertexArrays(1, &gridChunksVAO);
    if (gridChunksVBO) glDeleteBuffers(1, &gridChunksVBO);
}

void ChunksRenderer::updateChunkPosition(const unsigned int c, const void* data) {
    ASSERT(c < chunkCapacity, "Requested chunk " << c << " out of bounds.");
    ASSERT(data != nullptr, "Data pointer must not be nullptr.");

    size_t size = sizeof(glm::vec2);
    size_t offset = c * size;
    glBindBuffer(GL_ARRAY_BUFFER, chunkPositionsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunksRenderer::updateChunkTiles(const unsigned int c, const void* data) {
    ASSERT(c < chunkCapacity, "Requested chunk " << c << " out of bounds.");
    ASSERT(data != nullptr, "Data pointer must not be nullptr.");

    size_t size = CHUNK_TILE_COUNT * sizeof(GLubyte);
    size_t offset = c * size;
    glBindBuffer(GL_ARRAY_BUFFER, chunkTileTypesVBO);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunksRenderer::render(const Camera& camera) {

    // get camera view projection matrix
    const glm::mat4& vp =  camera.getViewProjMatrix();

    // run chunk sprites pipeline
    spritesShader.use();
    spritesShader.setUniformMatrix4fv("uVP", 1, vp);
    spritesTexture.bind();
    glBindVertexArray(spritesVAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, tileCapacity);

    // run grid tiles pipeline
    if (showGridTiles) {
        gridTilesShader.use();
        gridTilesShader.setUniformMatrix4fv("uVP", 1, vp);
        glLineWidth(GRID_TILE_LINEWIDTH);
        glBindVertexArray(gridTilesVAO);
        glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, tileCapacity);
    }

    // run grid sectors pipeline
    if (showGridSectors) {
        gridSectorsShader.use();
        gridSectorsShader.setUniformMatrix4fv("uVP", 1, vp);
        glLineWidth(GRID_SECTOR_LINEWIDTH);
        glBindVertexArray(gridSectorsVAO);
        glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, sectorCapacity);
    }

    // run grid chunks pipeline
    if (showGridChunks) {
        gridChunksShader.use();
        gridChunksShader.setUniformMatrix4fv("uVP", 1, vp);
        glLineWidth(GRID_CHUNK_LINEWIDTH);
        glBindVertexArray(gridChunksVAO);
        glDrawArraysInstanced(GL_LINE_LOOP, 0, 5, chunkCapacity);
    }

    // unbind
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

} // namespace Graphics