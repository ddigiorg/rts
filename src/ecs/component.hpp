#pragma once

#include "ecs/types.hpp"
#include "utils/assert.hpp"

#include <array>

namespace ECS {

// =============================================================================
// Component
// =============================================================================

class Component {
public:
    friend class ComponentManager;

    Component()
        : id(COMPONENT_ID_NULL),
          mask(COMPONENT_MASK_NULL),
          size(COMPONENT_SIZE_NULL),
          offset(COMPONENT_SIZE_NULL) {};

    bool isTag() const { return size == 0; }

    ComponentID   getID()     const { return id; }
    ComponentMask getMask()   const { return mask; }
    ComponentSize getSize()   const { return size; }
    ComponentSize getOffset() const { return offset; }

    void setOffset(ComponentSize offset_) { offset = offset_; }

private:
    ComponentID   id;     // unique component identifier
    ComponentMask mask;   // bitmask with a single set bit at "id" (e.g. 1 << id)
    ComponentSize size;   // size in bytes of a single component element
    ComponentSize offset; // chunk array offset (used in Archetype)
};

// =============================================================================
// Component Manager
// =============================================================================

class ComponentManager {
public:
    ComponentManager() : components({}), count{0} {}

    // component registration
    template <typename T>
    Component& registerComponent();

    // queries
    template <typename T>
    bool hasComponent() const;
    bool hasComponent(ComponentID id) const;
    template <typename T>
    bool isTag() const;
    bool isTag(ComponentID id) const;

    // getters
    template <typename T>
    Component getComponent();
    Component getComponent(ComponentID id);

    // miscellaneous
    void print();

private:
    std::array<Component, COMPONENT_CAPACITY> components;
    ComponentID count;
};

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

void ComponentManager::print() {
    std::cout << "components:" << std::endl;
    for (ComponentID id = 0; id < count; id++) {
        const Component& c = components[id];
        std::cout << "  - id: "   << (int)c.id   << std::endl;
        std::cout << "    mask: " <<      c.mask << std::endl;
        std::cout << "    size: " <<      c.size << std::endl;
    }
}

} // namespace ECS