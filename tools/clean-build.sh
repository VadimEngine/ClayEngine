#!/bin/bash

# Echo a message
echo "Cleaning up build directory..."

# Remove all directories inside the 'build' folder, including hidden ones
for dir in ./build/* ./build/.*; do
    # Skip special entries '.' and '..'
    if [ -d "$dir" ] && [[ "$dir" != "./build/." && "$dir" != "./build/.." ]]; then
        echo "Removing directory: $dir"
        rm -rf "$dir"
    fi
done

# Remove all files inside the 'build' folder, including hidden ones
find ./build -mindepth 1 -type f -exec rm -f {} \;

echo "Cleanup completed."