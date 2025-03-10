// #include "core/hierarchical_grid_old.hpp"
#include "core/hierarchical_grid.hpp"

#include "utils/random.hpp"
#include "utils/timer.hpp"

int main() {
    RandomFloatGenerator rng(0, 1024.0f * 32);
    Timer timer;
    World world;

    for (int i = 0; i < 100000; i++)
        world.addEntity({rng.get(), rng.get()}, EntityType::Entity1);

    // std::cout << world.getChunkCount() << std::endl;
    std::cout << world.getSectorCount() << std::endl;

    timer.reset();

    world.runCollision();
    // world.runCollisionParallel();

    std::cout << timer.elapsed() << std::endl;

    return 0;
}