# RTS

An open-world real time strategy game.

## Requirements

- [Git](https://git-scm.com/)
- [CMake](https://cmake.org/)
- [OpenGL](https://www.opengl.org/)
- A C/C++ Compiler of your choice:
   - Linux: [GCC/G++](https://gcc.gnu.org/)
   - Windows: [MSVC](https://visualstudio.microsoft.com/vs/)
   - MacOS: [Clang](https://clang.llvm.org/)

## Installation

NOTE: If you are on Windows you must use `Git Bash` or `wsl` to use the build tools located in `tools/`.

NOTE: You will need to edit the opengl paths in `tools/source-env.sh` before proceeding.

```bash
# source environment
. tools/1-source-env.sh

# get dependencies and compile game
./tools/2-download-dependencies.sh
./tools/3-build-dependencies.sh
./tools/4-build-game.sh

# run executable
./build/demos/Debug/game-2025-01-31.exe
```

TODO: Make the build process more robust for linux, windows, and macos.