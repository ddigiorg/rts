// Events.hpp
#pragma once

#include "ecs/Types.hpp"

struct OnCreate : ECS::Event {};
struct OnDestroy : ECS::Event {};
struct OnUpdate : ECS::Event {};
struct OnRender : ECS::Event {};