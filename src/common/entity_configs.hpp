#pragma once

#include "common/types.hpp"

constexpr EntityConfig ENTITY_RED {
    EntityType::RED,                           // type
    Vec4<float>(1.0f, 0.0f, 0.0f, 1.0f),       // renderColor
    BoundingBox(-16.0f, -16.0f, 16.0f, 16.0f), // renderXYBox
};

constexpr EntityConfig ENTITY_GREEN {
    EntityType::GREEN,                         // type
    Vec4<float>(0.0f, 1.0f, 0.0f, 1.0f),       // renderColor
    BoundingBox(-16.0f, -16.0f, 16.0f, 16.0f), // renderXYBox
};

constexpr EntityConfig ENTITY_BLUE {
    EntityType::BLUE,                          // type
    Vec4<float>(0.0f, 0.0f, 1.0f, 1.0f),       // renderColor
    BoundingBox(-16.0f, -16.0f, 16.0f, 16.0f), // renderXYBox
};

constexpr EntityConfig ENTITY_YELLOW {
    EntityType::YELLOW,                        // type
    Vec4<float>(1.0f, 1.0f, 0.0f, 1.0f),       // renderColor
    BoundingBox(-16.0f, -16.0f, 16.0f, 16.0f), // renderXYBox
};