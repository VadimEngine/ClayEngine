# Clay Engine
Early Alpha Clay Engine static library for developing games in C++. Currently supports an entity component system for game objects that are rendered in a Scene.

This libraries uses the following third party libraries.
### Third Party Libraries
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
    - For playing audio files
- sndfile
    - For loading audio files
- SOIL
    - For loading/saving images such as pngs or bmp

All the libraries are included in `./ThirdParty` directory as submodules. They are linked through cmake and can be all build with the bash script

```bash
./build-tool.sh -Build all
```

# Instructions 
With the third party libraries built, the Clay Static lib can be built with the following commands:

- To build (outputs to `\build` directory). `-DBUILD_TESTS=ON` is optional:
    - `cmake -S . -B build -DBUILD_TESTS=ON`
    - `cmake --build .\build\`

- Clean build for a clean build
    - `./clean-build`

- Run test:
    - `.\build\test\Debug\ClayEngineTest.exe`

A demo project for using this library can be seen [here](https://github.com/VadimEngine/ClayEngineDemo)