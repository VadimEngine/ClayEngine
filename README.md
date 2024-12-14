# Clay Engine
Early Alpha Clay Engine cross-platform static library for developing games in C++. Currently supports an entity component system for game objects that are rendered in custom Scenes using OpenGL. Works on windows and Linux (Tested on Arch and Fedora)

### Third Party Libraries
This libraries uses the following third party libraries:

- assimp
    - For loading 3d models
- freetype
    - For loading and rendering fonts and text
- GLEW
    - For managing and using OpenGL extensions
- GLFW
    - For creating windows, receiving input, and managing OpenGL contexts, 
- googletest
    - for unit test
- imgui
    - For prototype GUI
- OpenAL
    - For playing audio
- sndfile
    - For loading audio files
- SOIL
    - For loading/saving image files such as pngs or bmp

All the libraries are included in `./ThirdParty` directory as submodules.

# Instructions 
The Clay Static library can be built with the following commands:

- Initialize all third party submodules
    - `git submodule update --init --recursive`  

- To build (outputs to `/build` directory). `-DCLAY_BUILD_TESTS=ON` is optional:
    - `cmake -S . -B build -DCLAY_BUILD_TESTS=ON`
    - `cmake --build ./build/`

- Clean build directory for a clean build
    - `./tools/clean-build.sh`

- Run test (if build with -DCLAY_BUILD_TESTS=ON):
    - `./build/test/Debug/ClayEngineTest.exe`

A demo project for using this library can be seen [here](https://github.com/VadimEngine/ClayEngineDemo)
