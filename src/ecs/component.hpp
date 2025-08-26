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
    Component();
    void initialize(id_t id_, size_t size_);
    void initArrayInfo(size_t arrayOffset_, size_t arraySize_);
    bool isTag() const { return size == 0; }
    id_t getId() const { return id; }
    size_t getMask() const { return mask; }
    size_t getSize() const { return size; }
    size_t getArrayOffset() const { return arrayOffset; }
    size_t getArraySize() const { return arraySize; }

    static constexpr size_t CAPACITY = sizeof(mask_t) * 8;

private:
    id_t   id;          // unique component identifier
    mask_t mask;        // bitmask with a single set bit at "id" (e.g. 1 << id)
    size_t size;        // size in bytes of a single component element
    size_t arrayOffset; // offset in bytes from the chunk data buffer start
    size_t arraySize;   // size in bytes of the component array
};

Component::Component()
    : id(INVALID_ID), size(0), arrayOffset(0), arraySize(0) {}

void Component::initialize(id_t id_, size_t size_) {
    ASSERT(id_ < CAPACITY, "Component ids must be between 0 and 63.");
    id   = id_;
    size = size_;
    mask = mask_t(1) << id;
}

void Component::initArrayInfo(size_t arrayOffset_, size_t arraySize_) {
    arrayOffset = arrayOffset_;
    arraySize   = arraySize_;
}

} // namespace ECS