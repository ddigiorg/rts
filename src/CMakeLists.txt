# define the engine as a header-only (interface) library
add_library(engine INTERFACE)

# include directories
target_include_directories(engine INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FREETYPE2_INCLUDE_PATH}
    ${GLEW_INCLUDE_PATH}
    ${GLM_INCLUDE_PATH}
    ${OPENGL_INCLUDE_PATH}
    ${SDL2_CONFIG_PATH}
    ${SDL2_INCLUDE_PATH}
    ${STB_INCLUDE_PATH}
)

# TODO: need to leverage find_package instead of manually setting paths

# link libraries
target_link_libraries(engine INTERFACE
    ${SDL2_LIBRARY_PATH}/SDL2main.lib
    ${SDL2_LIBRARY_PATH}/SDL2.lib
    ${FREETYPE2_LIBRARY_PATH}/freetype.lib
    ${GLEW_LIBRARY_PATH}/glew32.lib
    ${OPENGL_LIBRARY_PATH}/OpenGL32.lib
)
