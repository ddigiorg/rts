// Assert.hpp
#pragma once

#include <iostream>
#include <sstream> // For std::ostringstream
#include <cstdlib> // For std::abort

#ifdef NDEBUG
    #define ASSERT(condition, message) ((void)0) // Disable in release builds
#else
    #define ASSERT(condition, message)                                        \
        do {                                                                  \
            if (!(condition)) {                                               \
                std::ostringstream oss;                                       \
                oss << "Assertion failed: (" #condition ") in " << __FILE__   \
                    << " at line " << __LINE__ << ": " << message;            \
                std::cerr << oss.str() << std::endl;                          \
                std::abort();                                                 \
            }                                                                 \
        } while (false)
#endif