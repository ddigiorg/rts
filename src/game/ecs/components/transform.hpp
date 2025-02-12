#pragma once

#include "engine/ecs/types.hpp"

using namespace ECS;

struct Transform : Component {
    float x, y; // 2d position in the world
    float w, h; // 2d size in the world
};