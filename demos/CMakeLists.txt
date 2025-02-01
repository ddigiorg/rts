# game-2025-01-31
set(DEMO ${CMAKE_CURRENT_SOURCE_DIR}/game-2025-01-31)
get_filename_component(DEMO_NAME ${DEMO} NAME)
add_executable(${DEMO_NAME} ${DEMO}/Main.cpp)
target_link_libraries(${DEMO_NAME} engine)
target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test-camera
# set(DEMO ${CMAKE_CURRENT_SOURCE_DIR}/test-camera)
# get_filename_component(DEMO_NAME ${DEMO} NAME)
# add_executable(${DEMO_NAME} ${DEMO}/Main.cpp)
# target_link_libraries(${DEMO_NAME} engine)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)