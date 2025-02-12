#pragma once

#include "engine/ecs/types.hpp"

using namespace ECS;

struct TexCoord : Component {
    float u, v; // 2d position in the texture
    float w, h; // 2d size in the texture
};