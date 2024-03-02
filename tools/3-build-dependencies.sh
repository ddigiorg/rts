#!/bin/bash

# build FreeType2
rm -rf $GAME_FREETYPE2_BUILD_PATH
echo "INFO: Generating FreeType2 build system..."
cmake -S $GAME_FREETYPE2_PATH -B $GAME_FREETYPE2_BUILD_PATH -D CMAKE_BUILD_TYPE=Release
echo "INFO: Building FreeType2..."
cmake --build $GAME_FREETYPE2_BUILD_PATH --config Release

# build SDL2
rm -rf $GAME_SDL2_BUILD_PATH
echo "INFO: Generating SDL2 build system..."
cmake -S $GAME_SDL2_PATH -B $GAME_SDL2_BUILD_PATH -D CMAKE_BUILD_TYPE=Release
echo "INFO: Building SDL2..."
cmake --build $GAME_SDL2_BUILD_PATH --config Release

# TODO: this isnt working...
# # build SDL2 TTF
# rm -rf $GAME_SDL2_TTF_BUILD_PATH
# echo "INFO: generating SDL2 TTF build system..."
# cmake -S $GAME_SDL2_TTF_PATH -B $GAME_SDL2_TTF_BUILD_PATH -D CMAKE_BUILD_TYPE=Release
# echo "INFO: building SDL2 TTF..."
# cmake --build $GAME_SDL2_TTF_BUILD_PATH --config Release
