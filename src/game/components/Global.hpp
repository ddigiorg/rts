// Global.hpp
#pragma once

#include "ecs/Types.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Cursor.hpp"
#include "graphics/SpriteRenderer.hpp"

struct Global : ECS::Component {
    GFX::Camera* camera;
    GFX::Cursor* cursor;
    GFX::SpriteRenderer* spriteRenderer;
};