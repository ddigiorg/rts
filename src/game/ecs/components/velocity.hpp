#pragma once

#include "ecs/types.hpp"

struct Velocity : ECS::Component {
    float x, y;
};