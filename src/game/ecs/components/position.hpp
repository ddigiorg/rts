#pragma once

#include "ecs/types.hpp"

struct Position : ECS::Component {
    float x, y;
};