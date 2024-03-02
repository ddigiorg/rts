#/!bin/bash

# helper function to modify drive prefix on windows
convert_git_bash_path_to_windows() {
    path=$1
    prefix=${path:0:2}
    drive=${prefix:1}
    if [ $prefix = /${drive,} ]; then
        path=${drive^}:${path:2}
    fi
    echo $path
}

# setup machine information
export GAME_OPERATING_SYSTEM=$(uname -o)
export GAME_MACHINE=$(uname -m)

# get path where this file resides
this_path=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
    this_path=$(convert_git_bash_path_to_windows $this_path)
fi

# setup game environment variables
export GAME_BASE_PATH=$(dirname $this_path)
export GAME_BUILD_PATH=$GAME_BASE_PATH/build
export GAME_DATA_PATH=$GAME_BASE_PATH/data
export GAME_EXTERNAL_PATH=$GAME_BASE_PATH/external
export GAME_SOURCE_PATH=$GAME_BASE_PATH/src

# setup FreeType2 environmant variables
export GAME_FREETYPE2_PATH=$GAME_EXTERNAL_PATH/freetype2
export GAME_FREETYPE2_BUILD_PATH=$GAME_FREETYPE2_PATH/build
export GAME_FREETYPE2_INCLUDE_PATH=$GAME_FREETYPE2_PATH/include
export GAME_FREETYPE2_LIBRARY_PATH=$GAME_FREETYPE2_BUILD_PATH/Release

# setup GLEW environment variables
export GAME_GLEW_PATH=$GAME_EXTERNAL_PATH/glew
export GAME_GLEW_INCLUDE_PATH=$GAME_GLEW_PATH/include
export GAME_GLEW_LIBRARY_PATH=$GAME_GLEW_PATH/lib/Release/x64
export GAME_GLEW_BINARY_PATH=$GAME_GLEW_PATH/bin/Release/x64

# setup GLM environment variables
export GAME_GLM_PATH=$GAME_EXTERNAL_PATH/glm
export GAME_GLM_INCLUDE_PATH=$GAME_GLM_PATH

# setup OpenGL
# TODO: make this smarter
export GAME_OPENGL_INCLUDE_PATH="C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um"
export GAME_OPENGL_LIBRARY_PATH="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64"
# export GAME_OPENGL_INCLUDE_PATH="C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um"
# export GAME_OPENGL_LIBRARY_PATH="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64"

# setup SDL2 environmant variables
export GAME_SDL2_PATH=$GAME_EXTERNAL_PATH/sdl2
export GAME_SDL2_BUILD_PATH=$GAME_SDL2_PATH/build
export GAME_SDL2_CONFIG_PATH=$GAME_SDL2_BUILD_PATH/include-config-release
export GAME_SDL2_INCLUDE_PATH=$GAME_SDL2_BUILD_PATH/include
export GAME_SDL2_LIBRARY_PATH=$GAME_SDL2_BUILD_PATH/Release
export GAME_SDL2_BINARY_PATH=$GAME_SDL2_BUILD_PATH/Release

# # setup SDL2 TTF environment variables
# export GAME_SDL2_TTF_PATH=$GAME_EXTERNAL_PATH/sdl2_ttf
# export GAME_SDL2_TTF_BUILD_PATH=$GAME_SDL2_TTF_PATH/build
# export GAME_SDL2_TTF_INCLUDE_PATH=$GAME_SDL2_TTF_PATH/include
# export GAME_SDL2_TTF_LIBRARY_PATH=$GAME_SDL2_TTF_PATH/lib/x64
# export GAME_SDL2_TTF_BINARY_PATH=$GAME_SDL2_TTF_PATH/lib/x64

# setup STB environment variables
export GAME_STB_PATH=$GAME_EXTERNAL_PATH/stb
export GAME_STB_INCLUDE_PATH=$GAME_STB_PATH
