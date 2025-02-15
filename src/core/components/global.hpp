#pragma once

#include "ecs/types.hpp"
#include "graphics/render/quads_renderer.hpp"

using namespace ECS;
using namespace Graphics;

struct Global : Component {
    QuadsRenderer* quadsRenderer;
};