#pragma once

#include "engine/ecs/types.hpp"
#include "engine/gfx/quad_renderer.hpp"

struct Global : ECS::Component {
    GFX::QuadRenderer* quadRenderer;
};