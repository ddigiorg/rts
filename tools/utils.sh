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

# TODO: figure out way to not need this...
# helper function to modify windows path back to git bash style
convert_windows_path_to_git_bash() {
    path=$1
    prefix=${path:0:1}
    drive=${prefix:0:1}
    if [ $prefix = ${drive^} ]; then
        path=/${drive,}${path:2}
    fi
    echo $path
}