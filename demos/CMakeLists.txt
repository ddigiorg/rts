# game_2025_01_31
set(DEMO ${CMAKE_CURRENT_SOURCE_DIR}/game_2025_01_31)
get_filename_component(DEMO_NAME ${DEMO} NAME)
add_executable(${DEMO_NAME} ${DEMO}/main.cpp)
target_link_libraries(${DEMO_NAME} engine)
target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)

# # test_camera
# set(DEMO ${CMAKE_CURRENT_SOURCE_DIR}/test_camera)
# get_filename_component(DEMO_NAME ${DEMO} NAME)
# add_executable(${DEMO_NAME} ${DEMO}/main.cpp)
# target_link_libraries(${DEMO_NAME} engine)
# target_include_directories(${DEMO_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)