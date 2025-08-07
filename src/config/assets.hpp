#pragma once

namespace Assets {
    namespace Fonts {
        constexpr const char* DEFAULT_FONT = "assets/fonts/CommitMono-400-Regular.otf";
    }

    namespace Graphics {
        constexpr const char* SPRITE_PNG = "assets/graphics/sprite.png";
    }

    namespace Shaders {
        constexpr const char* CURSOR_SELECT_VERT    = "assets/shaders/cursor-select.vert";
        constexpr const char* CURSOR_SELECT_FRAG    = "assets/shaders/cursor-select.frag";
        constexpr const char* FONT_VERT             = "assets/shaders/font.vert";
        constexpr const char* FONT_FRAG             = "assets/shaders/font.frag";
        constexpr const char* QUAD_INSTANCED_VERT   = "assets/shaders/quad-instanced.vert";
        constexpr const char* QUAD_INSTANCED_FRAG   = "assets/shaders/quad-instanced.frag";
        constexpr const char* SPRITE_INSTANCED_VERT = "assets/shaders/sprite-instanced.vert";
        constexpr const char* SPRITE_INSTANCED_FRAG = "assets/shaders/sprite-instanced.frag";
    }
}