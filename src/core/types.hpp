#pragma once

// NOTE: references for SDL3 states:
// - mouse states: https://wiki.libsdl.org/SDL3/SDL_MouseButtonFlags
// - keyboard states: https://wiki.libsdl.org/SDL3/SDL_Scancode
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <cstdint>
#include <string>
#include <vector>
#include <cmath>   // for std::floor
#include <limits>  // for std::numeric_limits
#include <utility> // for std::swap

// =============================================================================
// Types and Constants
// =============================================================================

using EntityID = uint16_t; // max 65534 active entities (last ID is always invalid or "null")

constexpr const EntityID ENTITY_NULL_ID    = std::numeric_limits<EntityID>::max();
constexpr const EntityID ENTITY_NULL_INDEX = std::numeric_limits<EntityID>::max();

constexpr const size_t TILE_PIXELS_X = 32;
constexpr const size_t TILE_PIXELS_Y = 32;
constexpr const size_t GRID_CELL_COUNT_X = 16;
constexpr const size_t GRID_CELL_COUNT_Y = 16;
constexpr const size_t GRID_CELL_COUNT = GRID_CELL_COUNT_X * GRID_CELL_COUNT_Y;
constexpr const size_t CELL_TILE_COUNT_X = 4;
constexpr const size_t CELL_TILE_COUNT_Y = 4;
constexpr const size_t CELL_PIXELS_X = CELL_TILE_COUNT_X * TILE_PIXELS_X;
constexpr const size_t CELL_PIXELS_Y = CELL_TILE_COUNT_Y * TILE_PIXELS_Y;

// =============================================================================
// Structs
// =============================================================================

struct Vec2f {
    union {
        float data[2];
        struct { float x, y; };
        struct { float w, h; };
        struct { float u, v; };
    };

    Vec2f() : Vec2f(0.0f, 0.0f) {}
    Vec2f(float x, float y) : x(x), y(y) {}
};

struct Vec4f {
    union {
        float data[4];
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };

    Vec4f() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Vec4f(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

struct Vec2i {
    union {
        int data[2];
        struct { int x, y; };
        struct { int w, h; };
    };

    Vec2i() : Vec2i(0, 0) {}
    Vec2i(int x, int y) : x(x), y(y) {}
};

struct Vec3i {
    union {
        int data[3];
        struct { int x, y, z; };
    };

    Vec3i() : Vec3i(0, 0, 0) {}
    Vec3i(int x, int y, int z) : x(x), y(y), z(z) {}
};

struct BoundingBox {
    Vec2f min, max;

    BoundingBox() : BoundingBox(0.0f, 0.0f, 32.0f, 32.0f) {}

    BoundingBox(float minX, float minY, float maxX, float maxY)
        : min(minX, minY), max(maxX, maxY) {}

    BoundingBox(const Vec2f& min, const Vec2f& max)
        : BoundingBox(min.x, min.y, max.x, max.y) {}
    
    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }

    Vec2f center() const {
        return { (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f };
    }

    void translate(Vec2f pos) {
        float halfWidth  = (max.x - min.x) * 0.5f;
        float halfHeight = (max.y - min.y) * 0.5f;

        min.x = pos.x - halfWidth;
        min.y = pos.y - halfHeight;
        max.x = pos.x + halfWidth;
        max.y = pos.y + halfHeight;
    }

    // bool intersects(const BoundingBox& other) const {
    //     return !(max.x < other.min.x || min.x > other.max.x ||
    //              max.y < other.min.y || min.y > other.max.y);
    // }    
};

// =============================================================================
// Helper Functions
// =============================================================================

Vec2i getGridFromWorld(const Vec2f& worldPos, const Vec2f& gridSize) {
    // TODO: could leverage bitshifts instead of divide
    int gridPosX = std::floor(worldPos.x / gridSize.x);
    int gridPosY = std::floor(worldPos.y / gridSize.y);
    return Vec2i(gridPosX, gridPosY);
}

Vec2f getWorldFromGrid(const Vec2i& gridPos, const Vec2f& gridSize) {
    float worldPosX = gridPos.x * gridSize.x;
    float worldPosY = gridPos.y * gridSize.y;
    return Vec2f(worldPosX, worldPosY);
}

template <typename T>
void swap_and_pop(std::vector<T>& vec, size_t index) {
    // if (index >= vec.size()) return; // TODO: make an assert instead
    std::swap(vec[index], vec.back());
    vec.pop_back();
}

// =============================================================================
// Graphics
// =============================================================================

// sprite sheet
constexpr const size_t SPRITE_SHEET_PIXELS_X = 384;
constexpr const size_t SPRITE_SHEET_PIXELS_Y = 192;
constexpr const float halfTexelX = 1.0f / float(SPRITE_SHEET_PIXELS_X);
constexpr const float halfTexelY = 1.0f / float(SPRITE_SHEET_PIXELS_Y);

// TODO
// struct GraphicsBuffer {
//     const GLuint index;
//     const GLint size;
//     const GLenum type;
//     const GLsizei stride;
//     const GLuint divisor;
//     GLuint vbo;
// };



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

// static const UnitConfig BALL_MAGENTA_CONFIG {
//     /*name:*/ "Ball Magenta",
//     /*scale:*/ Scale2f{32.0f, 32.0f},
//     /*hitbox:*/ Scale2f{32.0f, 32.0f},
//     /*frames:*/ AnimationOriented{
//         /*facing 0:*/ Animation {
//             /*frame 0:*/ computeTexCoord(128, 0,  64, 64),
//         },
//         /*facing 1:*/ Animation{
//             /*frame 0:*/ computeTexCoord(192, 0,  64, 64),
//         },
//         /*facing 2:*/ Animation{
//             /*frame 0:*/ computeTexCoord(256, 0,  64, 64),
//         },
//         /*facing 3:*/ Animation{
//             /*frame 0:*/ computeTexCoord(256, 0, -64, 64),
//         },
//         /*facing 4:*/ Animation{
//             /*frame 0:*/ computeTexCoord(192, 0, -64, 64),
//         },
//         /*facing 5:*/ Animation{
//             /*frame 0:*/ computeTexCoord(128, 0, -64, 64),
//         },
//         /*facing 6:*/ Animation{
//             /*frame 0:*/ computeTexCoord(  0, 0,  64, 64),
//         },
//         /*facing 7:*/ Animation{
//             /*frame 0:*/ computeTexCoord( 64, 0,  64, 64),
//         },
//     },
// };

// enum UnitType {
//     BALL_CYAN,
//     BALL_MAGENTA,
// };

// // TODO: should this be in UnitManager?
// static const std::array<UnitConfig, 2> UNIT_CONFIGS = {
//     BALL_CYAN_CONFIG,
//     BALL_MAGENTA_CONFIG,
// };