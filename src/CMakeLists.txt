# define as header-only (interface) library
add_library(game INTERFACE)

# find the required dependencies
find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
# find_package(SDL2 REQUIRED)
find_package(SDL3 REQUIRED)

# include directories
target_include_directories(game INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FASTNOISELITE_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${STB_INCLUDE_DIR}
)

# link libraries
target_link_libraries(game INTERFACE
    Freetype::Freetype
    GLEW::GLEW
    glm::glm
    OpenGL::GL
    SDL3::SDL3
)