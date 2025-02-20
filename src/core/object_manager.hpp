#pragma once

#include "utils/assert.hpp"

#include <cstdint>
#include <vector>
#include <limits> // for std::numeric_limits

namespace Core {

using ObjectID = uint32_t;
using DataIndex = uint32_t;

constexpr const uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();

class ObjectManager {
public:
    ObjectID create(const DataIndex& index);
    void remove(const ObjectID& id);
    void set(const ObjectID& id, const DataIndex& index);
    DataIndex get(const ObjectID& id);

private:
    std::vector<DataIndex> dataIndices;
    std::vector<ObjectID> recycledIDs;
};

ObjectID ObjectManager::create(const DataIndex& index = INVALID_INDEX) {
    ObjectID id;
    if (!recycledIDs.empty()) {
        id = recycledIDs.back();
        recycledIDs.pop_back();
        dataIndices[id] = index;
    } else {
        id = (ObjectID)dataIndices.size();
        dataIndices.push_back(index);
    }
    return id;
}

void ObjectManager::remove(const ObjectID& id) {
    ASSERT(id < dataIndices.size(), "Invalid ObjectID " << id);

    dataIndices[id] = INVALID_INDEX;
    recycledIDs.push_back(id);
}

void ObjectManager::set(const ObjectID& id, const DataIndex& index) {
    ASSERT(id < dataIndices.size(), "Invalid ObjectID " << id);
    ASSERT(dataIndices[id] != INVALID_INDEX, "Invalid ObjectID " << id);

    dataIndices[id] = index;
}

DataIndex ObjectManager::get(const ObjectID& id) {
    ASSERT(id < dataIndices.size(), "Invalid ObjectID " << id);
    ASSERT(dataIndices[id] != INVALID_INDEX, "Invalid ObjectID " << id);

    return dataIndices[id];
}

} // namespace Core