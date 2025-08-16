#pragma once

#include "utils/assert.hpp"

// NOTE: references for SDL3 states:
// - mouse states: https://wiki.libsdl.org/SDL3/SDL_MouseButtonFlags
// - keyboard states: https://wiki.libsdl.org/SDL3/SDL_Scancode
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <cstdint>
#include <vector>
#include <limits>  // for std::numeric_limits
#include <cmath>   // for std::floor
#include <utility> // for std::swap

// =============================================================================
// Vec2<T>
// =============================================================================

template <typename T>
struct Vec2 {
    union {
        T data[2];
        struct { T x, y; };
        struct { T w, h; };
        struct { T u, v; };
    };
    constexpr Vec2() : x(0), y(0) {}
    constexpr Vec2(T x_, T y_) : x(x_), y(y_) {}
};

// =============================================================================
// Vec4<T>
// =============================================================================

template <typename T>
struct Vec4 {
    union {
        T data[4];
        struct { T r, g, b, a; };
    };
    constexpr Vec4() : r(0), g(0), b(0), a(0) {}
    constexpr Vec4(T r_, T g_, T b_, T a_) : r(r_), g(g_), b(b_), a(a_) {}
};
// =============================================================================
// BoundingBox
// =============================================================================

struct BoundingBox {
    Vec2<float> min, max;

    constexpr BoundingBox() : BoundingBox(0.0f, 0.0f, 32.0f, 32.0f) {}

    constexpr BoundingBox(float minX, float minY, float maxX, float maxY)
        : min(minX, minY), max(maxX, maxY) {}

    constexpr BoundingBox(const Vec2<float>& min, const Vec2<float>& max)
        : BoundingBox(min.x, min.y, max.x, max.y) {}
    
    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }

    Vec2<float> center() const {
        return { (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f };
    }

    void translate(Vec2<float> pos) {
        min.x += pos.x;
        min.y += pos.y;
        max.x += pos.x;
        max.y += pos.y;
    }

    // void translate(Vec2<float> pos) {
    //     float halfWidth  = (max.x - min.x) * 0.5f;
    //     float halfHeight = (max.y - min.y) * 0.5f;

    //     min.x = pos.x - halfWidth;
    //     min.y = pos.y - halfHeight;
    //     max.x = pos.x + halfWidth;
    //     max.y = pos.y + halfHeight;
    // }

    // bool intersects(const BoundingBox& other) const {
    //     return !(max.x < other.min.x || min.x > other.max.x ||
    //              max.y < other.min.y || min.y > other.max.y);
    // }    
};

// =============================================================================
// Window
// =============================================================================

constexpr const char* WINDOW_TITLE                = "Game";
constexpr const int32_t WINDOW_DEFAULT_SIZE_X     = 800; // pixels
constexpr const int32_t WINDOW_DEFAULT_SIZE_Y     = 600; // pixels
constexpr const int32_t WINDOW_DEFAULT_MIN_SIZE_X = 320; // pixels
constexpr const int32_t WINDOW_DEFAULT_MIN_SIZE_Y = 200; // pixels

// =============================================================================
// Tile
//
// The fundamental square unit in the game world.
// =============================================================================

// constexpr const index32_t TILE_INDEX_NULL = std::numeric_limits<index32_t>::max();

constexpr const int32_t TILE_PIXELS_X = 32;
constexpr const int32_t TILE_PIXELS_Y = 32;

// =============================================================================
// Sector
//
// Grids of tiles in the game world representing a subset of a chunk.
// =============================================================================

// constexpr const index32_t SECTOR_INDEX_NULL = std::numeric_limits<index32_t>::max();

constexpr const int32_t SECTOR_TILES_X  = 4;
constexpr const int32_t SECTOR_TILES_Y  = 4;
constexpr const int32_t SECTOR_PIXELS_X = SECTOR_TILES_X * TILE_PIXELS_Y;
constexpr const int32_t SECTOR_PIXELS_Y = SECTOR_TILES_X * TILE_PIXELS_Y;

// =============================================================================
// Chunk
//
// Grids of tiles in the game world representing a subset of the map.
// =============================================================================

using ChunkHash = uint32_t;
using ChunkGrid = Vec2<int16_t>;

constexpr ChunkHash hashChunkGrid(const ChunkGrid& pos) noexcept {
    // Using large primes for better unordered_map performance
    return (static_cast<ChunkHash>(pos.x) * 73856093u) ^
           (static_cast<ChunkHash>(pos.y) * 19349663u);
}

constexpr ChunkGrid CHUNK_GRID_NULL{
    std::numeric_limits<int16_t>::max(),
    std::numeric_limits<int16_t>::max()
};

constexpr ChunkHash CHUNK_HASH_NULL = hashChunkGrid(CHUNK_GRID_NULL);

struct ChunkHashFunctor {
    size_t operator()(const ChunkGrid& pos) const noexcept {
        return static_cast<size_t>(hashChunkGrid(pos));
    }
};

struct ChunkEqualFunctor {
    bool operator()(const ChunkGrid& a, const ChunkGrid& b) const noexcept{
        return a.x == b.x && a.y == b.y;
    }
};

constexpr const int32_t CHUNK_TILES_X   = 32;
constexpr const int32_t CHUNK_TILES_Y   = 32;
constexpr const int32_t CHUNK_SECTORS_X = CHUNK_TILES_X / SECTOR_TILES_X;
constexpr const int32_t CHUNK_SECTORS_Y = CHUNK_TILES_Y / SECTOR_TILES_Y;
constexpr const int32_t CHUNK_PIXELS_X  = CHUNK_TILES_X * TILE_PIXELS_X;
constexpr const int32_t CHUNK_PIXELS_Y  = CHUNK_TILES_Y * TILE_PIXELS_Y;

// =============================================================================
// Map
//
// A singular grid of tiles in the game world representing the entire world.
// =============================================================================

// constexpr const chunk_t MAP_CHUNK_MIN_X = std::numeric_limits<chunk_t>::min() + 1;
// constexpr const chunk_t MAP_CHUNK_MIN_Y = std::numeric_limits<chunk_t>::min() + 1;
// constexpr const chunk_t MAP_CHUNK_MAX_X = std::numeric_limits<chunk_t>::max() - 1;
// constexpr const chunk_t MAP_CHUNK_MAX_Y = std::numeric_limits<chunk_t>::max() - 1;

constexpr const int32_t MAP_CHUNK_MIN_X = -2;
constexpr const int32_t MAP_CHUNK_MIN_Y = -2;
constexpr const int32_t MAP_CHUNK_MAX_X =  2;
constexpr const int32_t MAP_CHUNK_MAX_Y =  2;
constexpr const int32_t MAP_CHUNKS_X = MAP_CHUNK_MAX_X - MAP_CHUNK_MIN_X + 1;
constexpr const int32_t MAP_CHUNKS_Y = MAP_CHUNK_MAX_Y - MAP_CHUNK_MIN_Y + 1;

constexpr const int32_t MAP_SECTOR_MIN_X = MAP_CHUNK_MIN_X * CHUNK_SECTORS_X;
constexpr const int32_t MAP_SECTOR_MIN_Y = MAP_CHUNK_MIN_Y * CHUNK_SECTORS_Y;
constexpr const int32_t MAP_SECTOR_MAX_X = (MAP_CHUNK_MAX_X + 1) * CHUNK_SECTORS_X - 1;
constexpr const int32_t MAP_SECTOR_MAX_Y = (MAP_CHUNK_MAX_Y + 1) * CHUNK_SECTORS_Y - 1;
constexpr const int32_t MAP_SECTORS_X = MAP_SECTOR_MAX_X - MAP_SECTOR_MIN_X + 1;
constexpr const int32_t MAP_SECTORS_Y = MAP_SECTOR_MAX_Y - MAP_SECTOR_MIN_Y + 1;

constexpr const int32_t MAP_TILE_MIN_X = MAP_CHUNK_MIN_X * CHUNK_TILES_X;
constexpr const int32_t MAP_TILE_MIN_Y = MAP_CHUNK_MIN_Y * CHUNK_TILES_Y;
constexpr const int32_t MAP_TILE_MAX_X = (MAP_CHUNK_MAX_X + 1) * CHUNK_TILES_X - 1;
constexpr const int32_t MAP_TILE_MAX_Y = (MAP_CHUNK_MAX_Y + 1) * CHUNK_TILES_Y - 1;
constexpr const int32_t MAP_TILES_X = MAP_TILE_MAX_X - MAP_TILE_MIN_X + 1;
constexpr const int32_t MAP_TILES_Y = MAP_TILE_MAX_Y - MAP_TILE_MIN_Y + 1;

// =============================================================================
// Coordinates
// =============================================================================

// Vec2i getGridFromWorld(const Vec2f& worldPos, const Vec2f& gridSize) {
//     // TODO: could leverage bitshifts instead of divide
//     int gridPosX = std::floor(worldPos.x / gridSize.x);
//     int gridPosY = std::floor(worldPos.y / gridSize.y);
//     return Vec2i(gridPosX, gridPosY);
// }

// Vec2f getWorldFromGrid(const Vec2i& gridPos, const Vec2f& gridSize) {
//     float worldPosX = gridPos.x * gridSize.x;
//     float worldPosY = gridPos.y * gridSize.y;
//     return Vec2f(worldPosX, worldPosY);
// }

// =============================================================================
// Cursor
// =============================================================================

constexpr Vec4<float> CURSOR_BOX_SELECT_COLOR = {1.0f, 1.0f, 1.0f, 1.0f};

// =============================================================================
// Entity
// =============================================================================

// NOTE: max 65534 active entities (last ID is always invalid or "null")
using EntityID  = uint16_t;
using EntityIdx = uint16_t;

constexpr const EntityID  ENTITY_ID_NULL  = std::numeric_limits<EntityID>::max();
constexpr const EntityIdx ENTITY_IDX_NULL = std::numeric_limits<EntityIdx>::max();

enum class EntityType {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    SIZE,
};

struct EntityConfig {
    EntityType type;
    Vec4<float> color;
    BoundingBox renderXYBox;
};

template <typename T>
inline void entityDataRemove(std::vector<T>& vec, size_t index) {
    ASSERT(index < vec.size(), "Index out of bounds.");
    vec[index] = std::move(src.back());
    vec.pop_back();
}

template <typename T>
void entityDataMoveTo(std::vector<T>& dst, std::vector<T>& src,  size_t index) {
    ASSERT(index < src.size(), "Index out of bounds.");
    dst.push_back(std::move(src[index]));
    src[index] = std::move(src.back());
    src.pop_back();
}

// =============================================================================
// User Input
// =============================================================================

// TODO: put in input/types.h

struct WindowInput {
    bool resized = false;
    int width = 0;
    int height = 0;
};

struct MouseWheelInput {
    bool moved = false;
    float x = 0.0f;
    float y = 0.0f;
};

struct MouseInput {
    bool moved = false;
    float x = 0.0f;
    float y = 0.0f;
    float xrel = 0.0f;
    float yrel = 0.0f;
    uint32_t buttons;
    MouseWheelInput wheel;
};

struct KeyboardInput {
    int numkeys = 0;
    const bool* buttons = nullptr;
};

struct UserInput {
    bool quit = false;
    WindowInput window;
    MouseInput  mouse;
    KeyboardInput keyboard;
};

// =============================================================================
// Game State
// =============================================================================

// TODO: should I rename frame_state.hpp to game_states.hpp?
struct GameStates {
    bool isDebugActive = false;
    bool isSelecting = false;
};

// TODO: need to make game events more robust
struct SelectEvent {
    bool isActive = false;
    float boxBegX = 0.0f; // mouse coords
    float boxBegY = 0.0f; // mouse coords
    float boxEndX = 0.0f; // mouse coords
    float boxEndY = 0.0f; // mouse coords
};

// TODO: need to make game events more robust
struct GameEvents {
    SelectEvent select;
};

struct FrameState {
    float dt = 1.0f;
    UserInput input; // TODO: add inputPrev
    GameStates states;
    GameEvents events;
};

// =============================================================================
// Graphics
// =============================================================================

// // sprite sheet
// constexpr const size_t SPRITE_SHEET_PIXELS_X = 384;
// constexpr const size_t SPRITE_SHEET_PIXELS_Y = 192;
// constexpr const float halfTexelX = 1.0f / float(SPRITE_SHEET_PIXELS_X);
// constexpr const float halfTexelY = 1.0f / float(SPRITE_SHEET_PIXELS_Y);

// struct TexCoord4f {
//     float u, v, w, h;

//     TexCoord4f(float u = 0.0f, float v = 0.0f, float w = 0.0f, float h = 0.0f)
//         : u(u), v(v), w(w), h(h) {}
// };

// struct SpriteSheet {
//     float sizeX; // pixels
//     float sizeY; // pixels
//     std::string filepath;
// };

// inline TexCoord4f computeTexCoord(int pu, int pv, int pw, int ph) {
//     // NOTE: using half texel offsets to avoid texture bleeding
//     float tu = (float(pu) + 0.5f) * halfTexelX;
//     float tv = (float(pv) + 0.5f) * halfTexelY;
//     float tw = (float(pw) - 1.0f) * halfTexelX;
//     float th = (float(ph) - 1.0f) * halfTexelY;
//     return TexCoord4f(tu, tv, tw, th);
// }

// =============================================================================
// Game Object Configs
// =============================================================================

// using Animation = std::vector<TexCoord4f>;
// using AnimationOriented = std::array<Animation, 8>;

// struct TileConfig {
//     Animation frames;
// };

// static const TileConfig GRASS_CONFIG {
//     /*frames:*/Animation{computeTexCoord( 64, 128,  64, 64)},
// };

// static const TileConfig WATER_CONFIG {
//     /*frames:*/Animation{computeTexCoord(128, 128,  64, 64)},
// };

// static const std::array<TileConfig, 2> TILE_CONFIGS {
//     GRASS_CONFIG,
//     WATER_CONFIG,
// };

// struct UnitConfig {
//     std::string name;
//     Scale2f scale;
//     Scale2f hitbox;
//     AnimationOriented frames;
// };

// static const UnitConfig BALL_CYAN_CONFIG {
//     /*name:*/ "Ball Cyan",
//     /*scale:*/ Scale2f{32.0f, 32.0f},
//     /*hitbox:*/ Scale2f{32.0f, 32.0f},
//     /*frames:*/ AnimationOriented{
//         /*facing 0:*/ Animation{
//             /*frame 0:*/ computeTexCoord(128, 64,  64, 64),
//         },
//         /*facing 1:*/ Animation{
//             /*frame 0:*/ computeTexCoord(192, 64,  64, 64),
//         },
//         /*facing 2:*/ Animation{
//             /*frame 0:*/ computeTexCoord(256, 64,  64, 64),
//         },
//         /*facing 3:*/ Animation{
//             /*frame 0:*/ computeTexCoord(256, 64, -64, 64),
//         },
//         /*facing 4:*/ Animation{
//             /*frame 0:*/ computeTexCoord(192, 64, -64, 64),
//         },
//         /*facing 5:*/ Animation{
//             /*frame 0:*/ computeTexCoord(128, 64, -64, 64),
//         },
//         /*facing 6:*/ Animation{
//             /*frame 0:*/ computeTexCoord(  0, 64,  64, 64),
//         },
//         /*facing 7:*/ Animation{
//             /*frame 0:*/ computeTexCoord( 64, 64,  64, 64),
//         },
//     },
// };
