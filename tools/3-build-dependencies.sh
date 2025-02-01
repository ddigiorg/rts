#!/bin/bash

# build FreeType2
echo "================================================================================"
echo "Building FreeType2"
echo "================================================================================"
rm -rf $GAME_FREETYPE2_BUILD_PATH
cmake -S $GAME_FREETYPE2_PATH -B $GAME_FREETYPE2_BUILD_PATH -D CMAKE_BUILD_TYPE=Release
cmake --build $GAME_FREETYPE2_BUILD_PATH --config Release

# build SDL2
echo "================================================================================"
echo "Building SDL2"
echo "================================================================================"
rm -rf $GAME_SDL2_BUILD_PATH
cmake -S $GAME_SDL2_PATH -B $GAME_SDL2_BUILD_PATH -D CMAKE_BUILD_TYPE=Release
cmake --build $GAME_SDL2_BUILD_PATH --config Release
