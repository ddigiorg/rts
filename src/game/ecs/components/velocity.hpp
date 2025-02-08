#pragma once

#include "engine/ecs/types.hpp"

struct Velocity : ECS::Component {
    float x, y;
};