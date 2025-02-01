#!/bin/bash

# download FreeType2
echo "================================================================================"
echo "Downloading FreeType2"
echo "================================================================================"
git clone https://github.com/freetype/freetype.git $GAME_FREETYPE2_PATH

# download GLEW
echo "================================================================================"
echo "Downloading GLEW"
echo "================================================================================"
if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
    zip_path=$GAME_GLEW_PATH.zip
    curl -kLSs https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip --output $zip_path
    echo "$GAME_DEPENDENCIES_PATH"
    unzip $zip_path -d $GAME_DEPENDENCIES_PATH
    mv $GAME_DEPENDENCIES_PATH/glew-2.2.0 $GAME_GLEW_PATH
    rm $zip_path
else
    tgz_path=$GAME_GLEW_PATH.tgz
    curl -kLSs https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz --output $tgz_path
    tar -xvzf $tgz_path -C $GAME_DEPENDENCIES_PATH
    mv $GAME_DEPENDENCIES_PATH/glew-2.2.0 $GAME_GLEW_PATH
    rm $tgz_path
fi

# download GLM
echo "================================================================================"
echo "Downloading GLM"
echo "================================================================================"
git clone https://github.com/g-truc/glm.git $GAME_GLM_PATH

# download SDL2
# TODO: figure out the clone options for path and package name
echo "================================================================================"
echo "Downloading SDL2"
echo "================================================================================"
git clone -b release-2.26.x https://github.com/libsdl-org/SDL.git $GAME_SDL2_PATH

# download STB
echo "================================================================================"
echo "Downloading STB"
echo "================================================================================"
git clone https://github.com/nothings/stb.git $GAME_STB_PATH
