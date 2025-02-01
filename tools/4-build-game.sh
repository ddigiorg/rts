#!/bin/bash

rm -rf $GAME_BUILD_PATH

# generate build system
echo "================================================================================"
echo "Building Game"
echo "================================================================================"
cmake \
    -S $GAME_ROOT_PATH \
    -B $GAME_BUILD_PATH \
    -D FREETYPE2_INCLUDE_PATH="$GAME_FREETYPE2_INCLUDE_PATH" \
    -D FREETYPE2_LIBRARY_PATH="$GAME_FREETYPE2_LIBRARY_PATH" \
    -D GLEW_INCLUDE_PATH="$GAME_GLEW_INCLUDE_PATH" \
    -D GLEW_LIBRARY_PATH="$GAME_GLEW_LIBRARY_PATH" \
    -D GLM_INCLUDE_PATH="$GAME_GLM_INCLUDE_PATH" \
    -D OPENGL_INCLUDE_PATH="$GAME_OPENGL_INCLUDE_PATH" \
    -D OPENGL_LIBRARY_PATH="$GAME_OPENGL_LIBRARY_PATH" \
    -D SDL2_CONFIG_PATH="$GAME_SDL2_CONFIG_PATH" \
    -D SDL2_INCLUDE_PATH="$GAME_SDL2_INCLUDE_PATH" \
    -D SDL2_LIBRARY_PATH="$GAME_SDL2_LIBRARY_PATH" \
    -D STB_INCLUDE_PATH="$GAME_STB_INCLUDE_PATH"
cmake --build $GAME_BUILD_PATH --config Debug

# TODO: might be smarter to place this in CMakeLists.txt?
# copy data directory into build
# cp -r $GAME_DATA_PATH $GAME_BUILD_PATH/Debug
