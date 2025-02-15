# save binaries and libraries to build/project/bin and build/project/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/demos/bin)

# ==============================================================================
# Demos
# ==============================================================================

# # test_ecs
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_ecs)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_game_loop
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_game_loop)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_input
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_input)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_render_chunks
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_render_chunks)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_render_markers
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_render_markers)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_render_sprites
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_render_sprites)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_ui
# set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_ui)
# get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME)
# add_executable(${DEMO_NAME} ${DEMO_SOURCE}/main.cpp)
# target_link_libraries(${DEMO_NAME} PRIVATE game)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# test_unit_selection
set(DEMO_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/test_unit_selection)
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
            "${GLEW_BINARIES_DIR}/glew32.dll"
            "$<TARGET_FILE_DIR:${DEMO_NAME}>"
    )
endif()