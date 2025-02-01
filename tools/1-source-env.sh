#/!bin/bash

# get path where this file resides
this_path=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# import the utilities script
source $this_path/utils.sh

# setup machine information
export GAME_OPERATING_SYSTEM=$(uname -o)
export GAME_MACHINE=$(uname -m)

if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
    this_path=$(convert_git_bash_path_to_windows $this_path)
fi

# setup game environment variables
export GAME_ROOT_PATH=$(dirname $this_path)
export GAME_BUILD_PATH=$GAME_ROOT_PATH/build
export GAME_DEPENDENCIES_PATH=$GAME_ROOT_PATH/dependencies
export GAME_SOURCE_PATH=$GAME_ROOT_PATH/src

# setup FreeType2 environmant variables
export GAME_FREETYPE2_PATH=$GAME_DEPENDENCIES_PATH/freetype2
export GAME_FREETYPE2_BUILD_PATH=$GAME_FREETYPE2_PATH/build
export GAME_FREETYPE2_INCLUDE_PATH=$GAME_FREETYPE2_PATH/include
export GAME_FREETYPE2_LIBRARY_PATH=$GAME_FREETYPE2_BUILD_PATH/Release

# setup GLEW environment variables
export GAME_GLEW_PATH=$GAME_DEPENDENCIES_PATH/glew
export GAME_GLEW_INCLUDE_PATH=$GAME_GLEW_PATH/include
export GAME_GLEW_LIBRARY_PATH=$GAME_GLEW_PATH/lib/Release/x64
export GAME_GLEW_BINARY_PATH=$GAME_GLEW_PATH/bin/Release/x64

# setup GLM environment variables
export GAME_GLM_PATH=$GAME_DEPENDENCIES_PATH/glm
export GAME_GLM_INCLUDE_PATH=$GAME_GLM_PATH

# setup OpenGL environment variables
# TODO: make this smarter
export GAME_OPENGL_INCLUDE_PATH="C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um"
export GAME_OPENGL_LIBRARY_PATH="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64"
# export GAME_OPENGL_INCLUDE_PATH="C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um"
# export GAME_OPENGL_LIBRARY_PATH="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x64"

# setup SDL2 environmant variables
export GAME_SDL2_PATH=$GAME_DEPENDENCIES_PATH/sdl2
export GAME_SDL2_BUILD_PATH=$GAME_SDL2_PATH/build
export GAME_SDL2_CONFIG_PATH=$GAME_SDL2_BUILD_PATH/include-config-release
export GAME_SDL2_INCLUDE_PATH=$GAME_SDL2_BUILD_PATH/include
export GAME_SDL2_LIBRARY_PATH=$GAME_SDL2_BUILD_PATH/Release
export GAME_SDL2_BINARY_PATH=$GAME_SDL2_BUILD_PATH/Release

# setup STB environment variables
export GAME_STB_PATH=$GAME_DEPENDENCIES_PATH/stb
export GAME_STB_INCLUDE_PATH=$GAME_STB_PATH

# add binaries to path
# TODO: kind of forceful but CMake is annoying...
if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
    PATH=$PATH:$(convert_windows_path_to_git_bash $GAME_SDL2_BINARY_PATH)
    PATH=$PATH:$(convert_windows_path_to_git_bash $GAME_GLEW_BINARY_PATH)
else
    PATH=$PATH:$GAME_SDL2_BINARY_PATH
    PATH=$PATH:$GAME_GLEW_BINARY_PATH
fi