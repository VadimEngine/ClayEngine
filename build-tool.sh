#!/bin/bash

# Function to clean the contents of the build folder but keep the folder
clean_build_folder() {
    local build_folder="./build"

    if [ -d "$build_folder" ]; then
        echo "Cleaning contents of '$build_folder'..."
        rm -rf "$build_folder"/*
        echo "Cleaned contents of '$build_folder'."
    else
        echo "Build folder '$build_folder' does not exist. Creating it..."
        mkdir -p "$build_folder"
    fi
}

# Function to build specified dependencies
build_dependency() {
    local dependency=$1

    case $dependency in
        assimp)
            echo "Building Assimp..."
            cmake -B ./ThirdParty/assimp -S ./ThirdParty/assimp -DBUILD_SHARED_LIBS=OFF
            cmake --build ./ThirdParty/assimp
            ;;
        freeType)
            echo "Building FreeType..."
            cmake -B ./ThirdParty/freetype/build -S ./ThirdParty/freetype/ -D CMAKE_BUILD_TYPE=Debug
            cmake --build ./ThirdParty/freetype/build
            ;;
        GLEW)
            echo "Building GLEW..."
            (cd ./ThirdParty/glew-cmake && ./cmake-testbuild.sh)
            ;;
        GLFW)
            echo "Building GLFW..."
            cmake -B ./ThirdParty/glfw/build -S ./ThirdParty/glfw/
            cmake --build ./ThirdParty/glfw/build
            ;;
        OpenAL)
            echo "Building OpenAL..."
            cmake -B ./ThirdParty/openal-soft/build -S ./ThirdParty/openal-soft/ -DLIBTYPE=STATIC
            cmake --build ./ThirdParty/openal-soft/build
            ;;
        sndfile)
            echo "Building sndfile..."
            cmake -B ./ThirdParty/libsndfile/build -S ./ThirdParty/libsndfile/
            cmake --build ./ThirdParty/libsndfile/build
            ;;
        SOIL)
            echo "Building SOIL..."
            cmake -B ./ThirdParty/soil/build -S ./ThirdParty/soil/
            cmake --build ./ThirdParty/soil/build
            ;;
        GLM)
            echo "Building GLM..."
            # Maybe build non-header-only mode
            ;;
        Clay)
            echo "Building Clay..."
            #cmake ./ThirdParty/Clay/ -DBUILD_SHARED_LIBS=OFF
            #cmake --build ./ThirdParty/Clay/
            ;;
        *)
            echo "Unknown dependency: $dependency" >&2
            exit 1
            ;;
    esac

    if [ $? -ne 0 ]; then
        echo "Build failed for $dependency!" >&2
        exit 1
    else
        echo "Build succeeded for $dependency!"
    fi
}

# Variables for arguments
CLEAN=false
BUILD_ITEMS=()

# Parse the arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        -Clean)
            CLEAN=true
            shift
            ;;
        -Build)
            shift
            while [[ "$#" -gt 0 && ! "$1" =~ ^- ]]; do
                BUILD_ITEMS+=("$1")
                shift
            done
            ;;
        *)
            echo "Unknown argument: $1" >&2
            exit 1
            ;;
    esac
done

# Handle "all" in the build items
if [[ " ${BUILD_ITEMS[@]} " =~ " all " ]]; then
    BUILD_ITEMS=("assimp" "freeType" "GLEW" "GLFW" "OpenAL" "sndfile" "SOIL" "GLM")
fi

# Clean the build folder if the -Clean flag is provided
if [ "$CLEAN" = true ]; then
    clean_build_folder
fi

# Build each specified item if any are provided
for item in "${BUILD_ITEMS[@]}"; do
    build_dependency "$item"
done

# Run CMake commands for the main project if no specific builds are requested
echo "Running CMake configuration..."
#cmake -S . -B build

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!" >&2
    exit 1
fi

echo "Building the project..."
#cmake --build ./build

if [ $? -ne 0 ]; then
    echo "Build failed!" >&2
    exit 1
else
    echo "Build succeeded!"
fi
