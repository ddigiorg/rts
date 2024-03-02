#!/bin/bash

# TODO: figure out way to not need this...
convert_windows_path_to_git_bash() {
    path=$1
    prefix=${path:0:1}
    drive=${prefix:0:1}
    if [ $prefix = ${drive^} ]; then
        path=/${drive,}${path:2}
    fi
    echo $path
}

# update path environment with paths to dlls
if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
    PATH=$PATH:$(convert_windows_path_to_git_bash $GAME_SDL2_BINARY_PATH)
    PATH=$PATH:$(convert_windows_path_to_git_bash $GAME_SDL2_TTF_BINARY_PATH)
    PATH=$PATH:$(convert_windows_path_to_git_bash $GAME_GLEW_BINARY_PATH)
else
    PATH=$PATH:$GAME_SDL2_BINARY_PATH
    PATH=$PATH:$GAME_SDL2_TTF_BINARY_PATH
    PATH=$PATH:$GAME_GLEW_BINARY_PATH
fi

# run game
$GAME_BUILD_PATH/Debug/Game.exe
