#pragma once

#include "common/types.hpp"
#include "utils/assert.hpp"

#include <vector>

struct EntityRecord {
    ChunkHash chunk;
    EntityIdx index;

    static constexpr EntityRecord Null() {
        return {CHUNK_HASH_NULL, ENTITY_IDX_NULL};
    }
};

class EntityManager {
public:
    EntityID registerEntity();
    void removeEntity(const EntityID id);

    const EntityRecord& getRecord(EntityID id) const;
    void setRecord(EntityID id, EntityRecord rec);

private:
    std::vector<EntityRecord> records; // Indexed by EntityID
    std::vector<EntityID> freeIds;
};

EntityID EntityManager::registerEntity() {

    EntityID id = ENTITY_ID_NULL;

    if (!freeIds.empty()) {
        id = freeIds.back();
        freeIds.pop_back();
    } else {
        id = records.size();
        records.emplace_back(EntityRecord::Null());
    }

    return id;
}

void EntityManager::removeEntity(const EntityID id) {
    ASSERT(id < records.size(), "EntityID does not exist.");
    records[id] = EntityRecord::Null();
    freeIds.push_back(id);
}

const EntityRecord& EntityManager::getRecord(EntityID id) const {
    ASSERT(id < records.size(), "EntityID does not exist.");
    return records[id];
}

void EntityManager::setRecord(EntityID id, EntityRecord rec) {
    ASSERT(id < records.size(), "EntityID does not exist.");
    records[id] = rec;
}