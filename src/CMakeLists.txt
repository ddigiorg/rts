# define the engine as a header-only (interface) library
add_library(engine INTERFACE)

# find the required dependencies
find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
# find_package(SDL2 REQUIRED)
find_package(SDL3 REQUIRED)

# include directories
target_include_directories(engine INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${STB_INCLUDE_DIR}
)

# link libraries
target_link_libraries(engine INTERFACE
    Freetype::Freetype
    GLEW::GLEW
    glm::glm
    OpenGL::GL
    SDL3::SDL3
)