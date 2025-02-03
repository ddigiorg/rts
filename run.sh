#/!bin/bash

filepath="$1"

if [ -f "$filepath" ]; then
    directory="$(dirname "$filepath")"
    filename="$(basename "$filepath")"
    cd "$directory"
    "./$filename"
else
    echo "Error: executable does not exist."
fi

