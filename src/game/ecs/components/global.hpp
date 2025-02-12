#pragma once

#include "engine/ecs/types.hpp"
#include "game/gfx/quads_renderer.hpp"

using namespace ECS;
using namespace GFX;

struct Global : Component {
    QuadsRenderer* quadsRenderer;
};