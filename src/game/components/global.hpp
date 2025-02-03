#pragma once

#include "ecs/types.hpp"
#include "graphics/camera.hpp"
#include "graphics/cursor.hpp"
#include "graphics/quad_renderer.hpp"

struct Global : ECS::Component {
    GFX::Camera* camera;
    GFX::Cursor* cursor;
    GFX::QuadRenderer* quadRenderer;
};