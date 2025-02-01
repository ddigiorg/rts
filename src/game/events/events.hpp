#pragma once

#include "ecs/types.hpp"

struct OnCreate : ECS::Event {};
struct OnDestroy : ECS::Event {};
struct OnUpdate : ECS::Event {};
struct OnRender : ECS::Event {};