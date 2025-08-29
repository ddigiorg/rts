#pragma once

#include "ecs/types.hpp"
#include "utils/assert.hpp"

#include <cstdint>
#include <limits> // for std::numeric_limits

namespace ECS {

#define typeof(C) getComponentID<C>()

using ComponentID = uint8_t;

constexpr const ComponentID COMPONENT_ID_NULL = std::numeric_limits<ComponentID>::max();
constexpr const size_t COMPONENT_CAPACITY = sizeof(mask_t) * 8;

struct IComponentData {};

inline ComponentID getNextComponentID() {
    static ComponentID counter = 0;
    return counter++;
}

template <typename C>
inline ComponentID getComponentID() {
    static ComponentID id = getNextComponentID();
    return id;
}

class Component {
public:
    friend class Archetype;
    friend class World;

    Component();

    bool isTag() const { return size == 0; }
    ComponentID getID() const { return id; }
    size_t getSize() const { return size; }
    mask_t getMask() const { return mask; }

private:
    ComponentID id; // unique component identifier
    uint16_t size;  // size in bytes of a single component element
    mask_t mask;    // bitmask with a single set bit at "id" (e.g. 1 << id)
};

Component::Component() : id(COMPONENT_ID_NULL), size(0) {}

} // namespace ECS