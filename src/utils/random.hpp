#pragma once

#include <random>

class RandomFloatGenerator {
public:
    RandomFloatGenerator(float min, float max, unsigned int seed = 0)
        : distribution(min, max), generator(seed) {}

    float get() { return distribution(generator); };

private:
    std::mt19937 generator; // random number generator (Mersenne Twister)
    std::uniform_real_distribution<float> distribution; // distribution in [min, max]
};