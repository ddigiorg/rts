#pragma once

#include "ecs/types.hpp"
#include "gfx/camera.hpp"
#include "gfx/cursor.hpp"
#include "gfx/quad_renderer.hpp"

struct Global : ECS::Component {
    GFX::Camera* camera;
    GFX::Cursor* cursor;
    GFX::QuadRenderer* quadRenderer;
};