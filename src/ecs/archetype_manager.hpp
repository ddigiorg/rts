#pragma once

#include "ecs/types.hpp"
#include "ecs/archetype.hpp"
#include "ecs/component.hpp"

#include <algorithm> // for std::sort
#include <deque>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace ECS {

class ArchetypeManager {
public:
    ArchetypeManager(ComponentManager& componentMgr_);

    template<typename... Components>
    Archetype& getOrCreateArchetype();

    // miscellaneous
    void print();

private:
    ComponentManager& componentMgr;

    std::deque<Archetype> archetypes;
    std::unordered_map<ArchetypeMask, ArchetypeID> maskToIDs;
};

ArchetypeManager::ArchetypeManager(ComponentManager& componentMgr_)
        : componentMgr(componentMgr_), archetypes({}), maskToIDs({}) {
    getOrCreateArchetype();
}

template<typename... Components>
Archetype& ArchetypeManager::getOrCreateArchetype() {

    // build a vector of registered components
    std::vector<Component> components;
    components.reserve(sizeof...(Components));

    // helper lambda to add one component type
    auto _addComponent = [&](auto typeTag) {
        using T = decltype(typeTag);
        Component& c = componentMgr.getComponent<T>();
        components.push_back(c);
    };

    // expand pack
    (_addComponent(Components{}), ...);

    // sort components by ID
    std::sort(
        components.begin(),
        components.end(),
        [](const Component& a, const Component& b) {
            return a.getID() < b.getID();
        }
    );

    // build archetype mask
    ArchetypeMask mask = 0;
    for (const Component& c : components) {
        mask |= (ArchetypeMask(1) << c.getID());
    }

    // if archetype already exists then return it
    auto it = maskToIDs.find(mask);
    if (it != maskToIDs.end()) {
        return archetypes[it->second];
    }

    // otherwise create new archetype
    ArchetypeID id = static_cast<ArchetypeID>(archetypes.size());
    archetypes.emplace_back(id, mask, std::move(components));
    maskToIDs.insert({mask, id});
    return archetypes.back();
}

void ArchetypeManager::print() {
    std::cout << "archetypes:" << std::endl;
    for (const Archetype& a : archetypes) {
        std::cout << "  - id: "           << a.getID()          << std::endl;
        std::cout << "    mask: "         << a.getMask()        << std::endl;
        std::cout << "    capacity: "     << a.getCapacity()    << std::endl;
        std::cout << "    components: "                         << std::endl;
        std::cout << "    - id: <EntityIDs>"                    << std::endl;
        std::cout << "      offset: "     << 0                  << std::endl;
        for (const Component& c : a.getComponents()) {
            std::cout << "    - id: "     << (int)c.getID()     << std::endl;
            std::cout << "      offset: " <<      c.getOffset() << std::endl;
        }
    }
}

} // namespace ECS