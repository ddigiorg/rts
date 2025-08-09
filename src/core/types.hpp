#pragma once

// NOTE: references for SDL3 states:
// - mouse states: https://wiki.libsdl.org/SDL3/SDL_MouseButtonFlags
// - keyboard states: https://wiki.libsdl.org/SDL3/SDL_Scancode
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <cstdint>
#include <string>

// =============================================================================
// Structs
// =============================================================================

struct Position2f {
    float x, y;

    Position2f() : Position2f(0.0f, 0.0f) {}

    Position2f(float x, float y)
        : x(x), y(y) {}
};

struct Position2i {
    int x, y;

    Position2i() : Position2i(0, 0) {}

    Position2i(int x, int y)
        : x(x), y(y) {}
};

struct Color4f {
    float r, g, b, a;

    Color4f() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

    Color4f(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a) {}
};

struct BoundingBox {
    Position2f min, max;

    BoundingBox() : BoundingBox(0.0f, 0.0f, 32.0f, 32.0f) {}

    BoundingBox(float minX, float minY, float maxX, float maxY)
        : min(minX, minY), max(maxX, maxY) {}

    BoundingBox(const Position2f& min, const Position2f& max)
        : BoundingBox(min.x, min.y, max.x, max.y) {}
    
    float width() const { return max.x - min.x; }
    float height() const { return max.y - min.y; }

    Position2f center() const {
        return { (min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f };
    }

    void translate(Position2f pos) {
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


// struct Position3f {
//     float x, y, z;

//     Position3f(float x = 0.0f, float y = 0.0f, float z = 0.0f)
//         : x(x), y(y), z(z) {}
// };



// struct Velocity2f {
//     float x, y;
// };


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