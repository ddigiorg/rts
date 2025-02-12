#pragma once

#include "engine/ecs/types.hpp"

using namespace ECS;

struct Color : Component {
    float r, g, b; // color channels (0.0f to 1.0f)
};