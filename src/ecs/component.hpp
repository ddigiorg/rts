#pragma once

#include "ecs/types.hpp"
#include "utils/assert.hpp"

namespace ECS {

#define typeof(C) getComponentId<C>()

struct IComponentData {};

inline id_t getNextComponentId() {
    static id_t counter = 0;
    return counter++;
}

template <typename C>
inline id_t getComponentId() {
    static id_t id = getNextComponentId();
    return id;
}

class Component {
public:
    friend class Archetype;
    friend class World;

    Component();

    bool isTag() const { return size == 0; }
    id_t getId() const { return id; }
    size_t getSize() const { return size; }
    size_t getMask() const { return mask; }

    static constexpr size_t CAPACITY = sizeof(mask_t) * 8;

private:
    id_t   id;   // unique component identifier
    size_t size; // size in bytes of a single component element
    mask_t mask; // bitmask with a single set bit at "id" (e.g. 1 << id)
};

Component::Component() : id(INVALID_ID), size(0) {}

} // namespace ECS