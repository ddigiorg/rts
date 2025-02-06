#pragma once

#include "ecs/types.hpp"

struct Color : ECS::Component {
    float r, g, b;
};