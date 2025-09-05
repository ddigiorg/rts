#pragma once

#include "ecs/types.hpp"

namespace ECS {

class Component {
public:
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

private:
    friend class ComponentManager;
    friend class Archetype;

    void _setOffset(ComponentSize offset) { this->offset = offset; }

    ComponentID   id;     // unique component identifier
    ComponentMask mask;   // bitmask with a single set bit at "id" (e.g. 1 << id)
    ComponentSize size;   // size in bytes of a single component element
    ComponentSize offset; // chunk array offset (used in Archetype)
};

} // namespace ECS