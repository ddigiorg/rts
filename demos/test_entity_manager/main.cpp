#include "common/types.hpp"
#include "core/entity_manager.hpp"

#include <iostream>

// TODO: need to develop this

int main() {
    EntityManager em;

    EntityID id0 = em.registerEntity();
    EntityID id1 = em.registerEntity();
    EntityID id2 = em.registerEntity();
    EntityID id3 = em.registerEntity();

    return 0;
}