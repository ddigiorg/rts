cmake_minimum_required(VERSION 3.16)
project(MyGameEngine)

# set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# add engine source code
add_subdirectory(src)

# add demos
add_subdirectory(demos)