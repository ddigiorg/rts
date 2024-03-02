# pragma once

#include <iostream>
#include <chrono>

class Timer {
public:
    Timer() : start(clock::now()) {};

    void reset() {
        start = clock::now();
    };

    double elapsed() const {
        auto end = clock::now();
        return std::chrono::duration<double>(end - start).count();
    };

private:
    using clock = std::chrono::high_resolution_clock; 
    std::chrono::time_point<clock> start;
};