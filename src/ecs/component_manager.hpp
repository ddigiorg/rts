#pragma once

#include "ecs/types.hpp"
#include "ecs/component.hpp"
#include "utils/assert.hpp"

#include <array>
#include <iostream>
#include <type_traits>

namespace ECS {

// =============================================================================
// ComponentManager Types
// =============================================================================

template <typename T>
constexpr bool IsComponentType =
    std::is_trivially_constructible_v<T> &&
    std::is_trivially_destructible_v<T> &&
    std::is_trivially_copyable_v<T> &&
    std::is_standard_layout_v<T>;

template <typename T>
constexpr bool IsTagType = std::is_empty_v<T>;

// =============================================================================
// ComponentManager
// =============================================================================

class ComponentManager {
public:
    ComponentManager() : components({}), count{0} {}

    template <typename T>
    bool hasComponent() const;
    bool hasComponent(ComponentID id) const;
    template <typename T>
    bool isTag() const;
    bool isTag(ComponentID id) const;

    template <typename T>
    Component getComponent();
    Component getComponent(ComponentID id);

    template <typename T>
    Component& registerComponent();

    void print();

private:
    std::array<Component, COMPONENT_CAPACITY> components;
    ComponentID count;
};

// =============================================================================
// ComponentManager Functions
// =============================================================================

template <typename T>
bool ComponentManager::hasComponent() const {
    return hasComponent(getComponentID<T>());
}

bool ComponentManager::hasComponent(ComponentID id) const {
    return components[id].id != COMPONENT_ID_NULL;
}

template <typename T>
bool ComponentManager::isTag() const {
    return isTag(getComponentID<T>());
}

bool ComponentManager::isTag(ComponentID id) const {
    return components[id].isTag();
}

template <typename T>
Component ComponentManager::getComponent() {
    return getComponent(getComponentID<T>());
}

Component ComponentManager::getComponent(ComponentID id) {
    ASSERT(hasComponent(id), "Component " << id << " does not exist.");
    return components[id];
}

template <typename T>
Component& ComponentManager::registerComponent() {
    static_assert(IsComponentType<T> || IsTagType<T>,
        "Type T is not a valid component (must be POD-like or an empty tag).");

    ComponentID id = getComponentID<T>();
    ASSERT(count < COMPONENT_CAPACITY, "Component registry full.");
    ASSERT(id < COMPONENT_CAPACITY, "Component id must be between 0 and 63");
    ASSERT(!hasComponent(id), "Component already registered.");

    Component& c = components[id];
    c.id   = id;
    c.mask = ComponentMask(1) << id;
    c.size = IsTagType<T> ? 0 : sizeof(T);
    count++;

    return c;
}

void ComponentManager::print() {
    std::cout << "components:" << std::endl;
    for (ComponentID id = 0; id < count; id++) {
        const Component& c = components[id];
        std::cout << "  - id: "   << (int)c.getID()   << std::endl;
        std::cout << "    mask: " <<      c.getMask() << std::endl;
        std::cout << "    size: " <<      c.getSize() << std::endl;
    }
}

} // namespace ECS