#pragma once

#include "engine/ecs/types.hpp"

struct Color : ECS::Component {
    float r, g, b;
};