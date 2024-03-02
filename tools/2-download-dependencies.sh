#!/bin/bash

# download FreeType2
echo "INFO: Downloading the FreeType2..."
git clone https://github.com/freetype/freetype.git $GAME_FREETYPE2_PATH

# download GLEW
echo "INFO: Downloading GLEW..."
if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
    zip_path=$GAME_GLEW_PATH.zip
    curl -kLSs https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip --output $zip_path
    echo "$GAME_EXTERNAL_PATH"
    unzip $zip_path -d $GAME_EXTERNAL_PATH
    mv $GAME_EXTERNAL_PATH/glew-2.2.0 $GAME_GLEW_PATH
    rm $zip_path
else
    tgz_path=$GAME_GLEW_PATH.tgz
    curl -kLSs https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz --output $tgz_path
    tar -xvzf $tgz_path -C $GAME_EXTERNAL_PATH
    mv $GAME_EXTERNAL_PATH/glew-2.2.0 $GAME_GLEW_PATH
    rm $tgz_path
fi

# download GLM
echo "INFO: Downloading GLM..."
git clone https://github.com/g-truc/glm.git $GAME_GLM_PATH

# download SDL2
# TODO: figure out the clone options for path and package name
echo "INFO: Downloading SDL2..."
git clone -b release-2.26.x https://github.com/libsdl-org/SDL.git $GAME_SDL2_PATH

# # download SDL2 TTF
# echo "INFO: Downloading SDL2 TTF..."
# if [ $GAME_OPERATING_SYSTEM == "Msys" ]; then
#     zip_path=$GAME_SDL2_TTF_PATH.zip
#     curl -kLSs https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.1/SDL2_ttf-2.20.1.zip --output $zip_path
#     unzip $zip_path -d $GAME_EXTERNAL_PATH
#     mv $GAME_EXTERNAL_PATH/SDL2_ttf-2.20.1 $GAME_SDL2_TTF_PATH
#     rm $zip_path
# else
#     tgz_path=$GAME_SDL2_TTF_PATH.tar.gz
#     curl -kLSs https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.1/SDL2_ttf-2.20.1.tar.gz --output $tgz_path
#     tar -xvzf $tgz_path -C $GAME_EXTERNAL_PATH
#     mv $GAME_EXTERNAL_PATH/SDL2_ttf-2.20.1 $GAME_SDL2_TTF_PATH
#     rm $tgz_path
# fi

# download STB
echo "INFO: Downloading STB..."
git clone https://github.com/nothings/stb.git $GAME_STB_PATH
