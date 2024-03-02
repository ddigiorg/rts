cmake_minimum_required(VERSION 3.0)
project(Game)

include_directories(
    ${FASTNOISELITE_INCLUDE_PATH}
    ${FREETYPE2_INCLUDE_PATH}
    ${GLEW_INCLUDE_PATH}
    ${GLM_INCLUDE_PATH}
    ${OPENGL_INCLUDE_PATH}
    ${SDL2_CONFIG_PATH}
    ${SDL2_INCLUDE_PATH}
    ${STB_INCLUDE_PATH}
)

add_executable(
    Game
    ${SOURCE_PATH}/ecs/chunk.cpp
    ${SOURCE_PATH}/ecs/archetype.cpp
    ${SOURCE_PATH}/ecs/world.cpp
    ${SOURCE_PATH}/main.cpp
)

target_link_libraries(
    Game
    ${FREETYPE2_LIBRARY_PATH}/freetype.lib
    ${GLEW_LIBRARY_PATH}/glew32.lib
    ${OPENGL_LIBRARY_PATH}/OpenGL32.lib
    ${SDL2_LIBRARY_PATH}/SDL2.lib
    ${SDL2_LIBRARY_PATH}/SDL2main.lib
)