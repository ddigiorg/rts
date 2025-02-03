#/!bin/bash

# ==============================================================================
# Script Start
# ==============================================================================

# get path where this file resides
this_path="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"

# get machine information
os="$(uname -o)"
arch="$(uname -m)"

# if using git bash then convert path to windows style
if [ $os == "Msys" ]; then
    this_path="$(cygpath -w "$this_path")"
fi

PROJECT_PATH="$this_path"
PROJECT_BUILD_PATH="$PROJECT_PATH/build"
DEPENDENCIES_SOURCE_PATH="$PROJECT_PATH/dependencies/source"
DEPENDENCIES_BUILD_PATH="$PROJECT_PATH/dependencies/build"
DEPENDENCIES_INSTALL_PATH="$PROJECT_PATH/dependencies/install"

# ==============================================================================
# Helper Functions
# ==============================================================================

# searches the dependencies directory for the matching substring (case-insensitive)
find_dependency_in_path() {
    substr=$1
    path=$2
    find "$path" -maxdepth 1 -type d -iname "*$substr*"
}

# downloads and extracts dependency from url
download_dependency() {
    name=$1
    url=$2

    # check if dependency already exists in dependencies path
    if [ -d "$(find_dependency_in_path "$name" "$DEPENDENCIES_SOURCE_PATH")" ]; then
        return 0
    fi

    cd "$DEPENDENCIES_SOURCE_PATH"
    filename="$(basename "$url")"

    # handle github urls
    if [[ "$filename" == *.git ]]; then
        git clone "$url"
        return 0
    fi

    # download the compressed file
    curl -kL "$url" --output "$filename"

    # check if the download was successful
    if [ ! -f "$filename" ]; then
        echo "Download failed or file not found."
        exit 1
    fi

    # Extract based on file type
    if [[ "$filename" == *.zip ]]; then
        unzip -o "$filename"
    elif [[ "$filename" == *.tar.gz || "$filename" == *.tgz ]]; then
        tar -xzf "$filename"
    else
        echo "Unknown file type. Not extracting."
        exit 1
    fi

    # remove the compressed file
    rm "$filename"
}

build_and_install_dependency() {
    name=$1

    # check if dependency already exists in dependencies build path
    if [ -d "$(find_dependency_in_path "$name" "$DEPENDENCIES_BUILD_PATH")" ]; then
        return 0
    fi

    source_path="$(find_dependency_in_path "$name" "$DEPENDENCIES_SOURCE_PATH")"
    basename="$(basename "$source_path")"
    build_path="$DEPENDENCIES_BUILD_PATH/$basename"
    install_path="$DEPENDENCIES_INSTALL_PATH/$basename"

    # account for GLEW's weird cmake location
    if [ $name = "glew" ]; then
        source_path="$source_path/build/cmake"
    fi

    # generate the dependency build system 
    cmake -S "$source_path" -B "$build_path" -D "CMAKE_BUILD_TYPE=Release"

    # build the dependency
    cmake --build "$build_path" --config "Release"

    # install the dependency
    cmake --install "$build_path" --config "Release" --prefix "$install_path"
}

# ==============================================================================
# Download Dependencies
# ==============================================================================

# create the dependencies directory
mkdir -p "$DEPENDENCIES_SOURCE_PATH"

# setup the download urls
freetype_url="https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.gz"
glew_url="https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip"
glm_url="https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"
sdl_url="https://github.com/libsdl-org/SDL/releases/download/release-2.30.12/SDL2-2.30.12.zip"
stb_url="https://github.com/nothings/stb.git"

# download
download_dependency "freetype" "$freetype_url"
download_dependency "glew" "$glew_url"
download_dependency "glm" "$glm_url"
download_dependency "sdl" "$sdl_url"
download_dependency "stb" "$stb_url"

# ==============================================================================
# Build and Install Dependencies
# ==============================================================================

# create the dependencies build directory
mkdir -p $DEPENDENCIES_BUILD_PATH
mkdir -p $DEPENDENCIES_INSTALL_PATH

# build and install
build_and_install_dependency "freetype"
build_and_install_dependency "glew"
build_and_install_dependency "glm"
build_and_install_dependency "sdl"

# ==============================================================================
# Build Project
# ==============================================================================

# create the project build directory
mkdir -p $PROJECT_BUILD_PATH

# get the dependency install paths
freetype_install_path="$(find_dependency_in_path "freetype" "$DEPENDENCIES_INSTALL_PATH")"
glew_install_path="$(find_dependency_in_path "glew" "$DEPENDENCIES_INSTALL_PATH")"
glm_install_path="$(find_dependency_in_path "glm" "$DEPENDENCIES_INSTALL_PATH")"
sdl_install_path="$(find_dependency_in_path "sdl" "$DEPENDENCIES_INSTALL_PATH")"
stb_source_path="$(find_dependency_in_path "stb" "$DEPENDENCIES_SOURCE_PATH")"

# generate the project build system 
cmake \
    -B $PROJECT_BUILD_PATH \
    -D PROJECT_DIR="$PROJECT_PATH" \
    -D CMAKE_PREFIX_PATH="$freetype_install_path;$glew_install_path;$glm_install_path;$sdl_install_path" \
    -D STB_INCLUDE_DIR="$stb_source_path" \
    -D SDL_BINARIES_DIR="$sdl_install_path/bin" \
    -D GLEW_BINARIES_DIR="$glew_install_path/bin"

# build the project
cmake --build $PROJECT_BUILD_PATH --config Debug 
