#pragma once

#include "engine/ecs/types.hpp"

struct Position : ECS::Component {
    float x, y;
};