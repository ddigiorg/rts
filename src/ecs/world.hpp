#pragma once

#include "ecs/types.hpp"
#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "utils/assert.hpp"

#include <algorithm> // for std::sort
#include <array>
#include <cstdint>
#include <iostream>
#include <limits> // for std::numeric_limits
#include <unordered_map>
#include <vector>

namespace ECS {

class World {
public:
    // World();
    // ~World();

    // entity functions
    // bool hasEntity(id_t entityId) const;
    id_t createEntity(const std::vector<id_t>& componentIds);
    // void removeEntity(id_t entityId);
    // template <typename C>
    // void insertEntityComponent();
    // template <typename C>
    // void removeEntityComponent();
    // template <typename C>
    // C* getEntityData(id_t entityId);
    // template <typename C>
    // void setEntityData(id_t entityId, const C& value);

    // component functions
    template <typename C>
    bool hasComponent() const;
    bool hasComponent(id_t id) const;
    template <typename C>
    bool isTag() const;
    bool isTag(id_t id) const;
    template <typename C>
    id_t registerComponent();
    template <typename C>
    id_t registerTag();

    // QueryIterator query(Query q);

    // print functions
    void print();
    // void printArchetypes();
    // void printChunks();
    void printComponents();
    void printEntities();

private:

    // entity functions
    id_t _nextEntity();
    void _freeEntity(id_t entityId);

    // archetype functions
    // id_t _getOrCreateArchetype(const std::vector<id_t>& componentIds);
    // void _pushChunkOnArchetype(id_t archetypeId);
    // void _popChunkOffArchetype(id_t archetypeId);
    // void _freeChunks();

    // entity data
    std::vector<Entity> entities{};
    std::vector<id_t> freeEntityIds{};
    id_t nextEntityId{0};

    // component data
    std::array<Component, Component::CAPACITY> components{};
    id_t componentCount{0};

    // archetype data
    // std::vector<Archetype> archetypes;
    // std::unordered_map<mask_t, id_t> archetypeMaskToId;

    // EventManager eventMgr;
    // QueryManager queryMgr;
    // SystemManager systemMgr;
};

// =============================================================================
// Constructor and Destructor
// =============================================================================

// World::World() {
//     _getOrCreateArchetype({}); // create default archetype with no components
// }

// World::~World() {
//     _freeChunks();
// }

// =============================================================================
// Entity Functions
// =============================================================================

// bool World::hasEntity(id_t entityId) const {
//     // TODO: need to detect if id is in freeIds
//     return entityId < entities.size();
// }

id_t World::createEntity(const std::vector<id_t>& componentIds) {
    id_t eId = _nextEntity();
    // id_t aId = _getOrCreateArchetype(componentIds);
    // std::vector<ChunkMeta >& chunkMeta = aChunkMeta[aId];
    // std::vector<ChunkData*>& chunkData = aChunkData[aId];

    // // allocate new chunk if no chunks exist in archetype
    // if (chunkData.empty()) {
    //     _pushChunkOnArchetype(aId);
    // }

    // TODO: archetype and chunk stuff here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    return eId;
}

// void World::removeEntity(id_t entityId) {
//     ASSERT(entityId < eNextId, "EntityId " << entityId << " does not exist.");
//     // TODO: archetype and chunk stuff here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//     _freeEntity(entityId);
// }

// void World::insertEntityComponent(id_t entityId, id_t componentId) {
//     archetypeMgr.moveEntityRight(entityId, componentId)
// }

// void World::removeEntityComponent(id_t entityId, id_t componentId) {
//     archetypeMgr.moveEntityLeft(entityId, componentId)
// }

id_t World::_nextEntity() {
    id_t id = INVALID_ID;

    if (!freeEntityIds.empty()) {
        id = freeEntityIds.back();
        freeEntityIds.pop_back();
        entities[id].setId(id);
        return id;
    }

    id = nextEntityId++;
    entities.emplace_back();
    entities.back().setId(id);
    return id;
}

void World::_freeEntity(id_t id) {
    ASSERT(id < nextEntityId, "EntityId " << id << " does not exist.");
    entities[id].invalidate();
    freeEntityIds.push_back(id);
}

// =============================================================================
// Component Functions
// =============================================================================

template <typename C>
bool World::hasComponent() const {
    return hasComponent(getComponentId<C>());
}

bool World::hasComponent(id_t id) const {
    return components[id].getId() != INVALID_ID;
}

template <typename C>
bool World::isTag() const {
    return isTag(getComponentId<C>());
}

bool World::isTag(id_t id) const {
    return components[id].isTag();
}

template <typename C>
id_t World::registerComponent() {
    id_t id = getComponentId<C>();
    static_assert(std::is_base_of<IComponentData, C>::value, "Component must inherit from IComponentData base.");
    ASSERT(componentCount < Component::CAPACITY, "Component registry full.");
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already registered.");
    components[id].initialize(id, sizeof(C));
    componentCount++;
    return id;
}

template <typename C>
id_t World::registerTag() {
    id_t id = getComponentId<C>();
    static_assert(std::is_base_of<IComponentData, C>::value, "Component must inherit from IComponentData base.");
    ASSERT(componentCount < Component::CAPACITY, "Component registry full.");
    ASSERT(!hasComponent(id), "Component \"" << typeid(C).name() << "\" already registered.");
    components[id].initialize(id, 0);
    componentCount++;
    return id;
}

// =============================================================================
// Archetype Functions
// =============================================================================

// id_t World::_getOrCreateArchetype(const std::vector<id_t>& componentIds) {

//     // sort input component ids to normalize
//     std::vector<id_t> sortedIds = componentIds;
//     std::sort(sortedIds.begin(), sortedIds.end());

//     // build mask
//     mask_t aMask = 0;
//     for (id_t cId : sortedIds) {
//         aMask |= (mask_t(1) << cId);
//     }

//     // check if archetype already exists and return it
//     auto it = aMaskToId.find(aMask);
//     if (it != aMaskToId.end()) {
//         return it->second;
//     }

//     // allocate new archetype
//     id_t aId = aNextId++;
//     ArchetypeRecord archetype {
//         /*   archetypeId=*/ aId,
//         /* archetypeMask=*/ aMask,
//         /*  openChunkIdx=*/ 0,
//         /*entityCapacity=*/ 0,
//         /*    components=*/ {},
//         /*     neighbors=*/ {},
//     };
//     archetype.components.reserve(sortedIds.size() + 1);

//     // add entity id component
//     size_t aSize = 0;
//     size_t idSize = sizeof(id_t);
//     archetype.components.push_back({ INVALID_ID, idSize, 0});
//     aSize += idSize;

//     // gather component metadata
//     for (id_t cId : sortedIds) {
//         const ComponentRecord& c = components[cId];
//         archetype.components.push_back(
//             ComponentRecord {
//                 /*componentId=*/ c.componentId,
//                 /*elementSize=*/ c.elementSize,
//                 /*arrayOffset=*/ 0,
//                 /*  arraySize=*/ 0, 
//             }
//         );
//         aSize += c.elementSize;
//     }

//     ASSERT(aSize < CHUNK_DATA_SIZE, "Archetype size exceeds chunk capacity");

//     archetype.entityCapacity = (aSize > 0) ? CHUNK_DATA_SIZE / aSize : CHUNK_DATA_SIZE;

//     // assign component offsets
//     size_t offset = 0;
//     size_t size = 0;
//     for (auto& ac : archetype.components) {
//         ac.arrayOffset = offset;
//         ac.arraySize = ac.elementSize * archetype.entityCapacity;
//         offset += ac.arraySize;
//     }

//     // append new archetype
//     aMaskToId[aMask] = aId;
//     archetypes.push_back(std::move(archetype));
//     aChunkMeta.push_back({});
//     aChunkData.push_back({});

//     return aId;
// }

// void World::_pushChunkOnArchetype(id_t archetypeId) {
//     ASSERT(archetypeId < archetypes.size(), "Archetype " << archetypeId << " does not exist.");
//     ArchetypeRecord& aRecord = archetypes[archetypeId];

//     ChunkMeta cMeta {
//         /*   archetypeId=*/ archetypeId,
//         /*      chunkIdx=*/ (idx_t)aChunkData[archetypeId].size(),
//         /*   entityCount=*/ 0,
//         /*entityCapacity=*/ aRecord.entityCapacity,
//         /*    cidToArray=*/ {},
//     };

//     ChunkData* cData = static_cast<ChunkData*>(malloc(CHUNK_DATA_SIZE));

//     for (const auto& c : aRecord.components) {
//         if (c.componentId == INVALID_ID) continue;
//         ChunkArrayRecord& ca = cMeta.cidToArray[c.componentId];
//         ca.addr = reinterpret_cast<void*>(reinterpret_cast<std::byte*>(cData) + c.arrayOffset);
//         ca.size = c.arraySize;
//     }

//     aChunkMeta[archetypeId].push_back(cMeta);
//     aChunkData[archetypeId].push_back(cData);
// }

// void World::_popChunkOffArchetype(id_t archetypeId) {

// }

// void World::_freeChunks() {
//     for (auto& chunks : aChunkData) {
//         for (auto chunk : chunks) {
//             free(chunk);
//         }
//     }
// }

// =============================================================================
// Print Functions
// =============================================================================

void World::print() {
    // printArchetypes();
    // printChunks();
    printComponents();
    printEntities();
}

// void World::printArchetypes() {
//     std::cout << "archetypes:" << std::endl;
//     for (const auto& a : archetypes) {
//         std::cout << "  - archetypeId: "    << a.archetypeId    << std::endl;
//         std::cout << "    archetypeMask: "  << a.archetypeMask  << std::endl;
//         std::cout << "    openChunkIdx: "   << a.openChunkIdx   << std::endl;
//         std::cout << "    entityCapacity: " << a.entityCapacity << std::endl;
//         std::cout << "    components: "                         << std::endl;
//         for (const auto& c : a.components) {
//             std::cout << "    - componentId: " << c.componentId << std::endl;
//             std::cout << "      elementSize: " << c.elementSize << std::endl;
//             std::cout << "      arrayOffset: " << c.arrayOffset << std::endl;
//             std::cout << "      arraySize: "   << c.arraySize   << std::endl;
//         }
//     }
// }

void World::printComponents() {
    std::cout << "components:" << std::endl;
    for (id_t id = 0; id < componentCount; id++) {
        const Component& c = components[id];
        std::cout << "  - id: "   << c.getId()   << std::endl;
        std::cout << "    size: " << c.getSize() << std::endl;
    }
}

void World::printEntities() {
    std::cout << "entities:" << std::endl;
    for (const Entity& e : entities) {
        std::cout << "  - id: "          << e.getId()        << std::endl;
        // std::cout << "  - archetypeId: " << e.archetypeId()  << std::endl;
        std::cout << "  - chunkIdx: "    << e.getChunkIdx()  << std::endl;
        std::cout << "  - entityIdx: "   << e.getEntityIdx() << std::endl;
    }
}

} // namespace ECS