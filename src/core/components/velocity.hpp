#pragma once

#include "ecs/types.hpp"

using namespace ECS;

struct Velocity : Component {
    float x, y; // 2d velocity in the world
};