# save binaries and libraries to build/project/bin and build/project/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos/bin)

# ==============================================================================
# Demos
# ==============================================================================

# TODO: make this a function
set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_entity_manager)
get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
target_link_libraries(${DEMO_NAME} PRIVATE game)
target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)


# ==============================================================================
# Copy files to bin
# ==============================================================================

# copy the assets folder to the build directory
add_custom_command(
    TARGET ${DEMO_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_DIR}/assets"
        "$<TARGET_FILE_DIR:${DEMO_NAME}>/assets"
)

# if on windows then copy required dlls
if(WIN32)
    # SDL2
    add_custom_command(
        TARGET ${DEMO_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            # "${SDL_BINARIES_DIR}/SDL2.dll"
            "${SDL_BINARIES_DIR}/SDL3.dll"
            "$<TARGET_FILE_DIR:${DEMO_NAME}>"
    )
    # GLEW
    add_custom_command(
        TARGET ${DEMO_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_BINARIES_DIR}/glew32d.dll" # TODO: remove d for release builds...
            "$<TARGET_FILE_DIR:${DEMO_NAME}>"
    )
endif()