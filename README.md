# Clay Engine
Early Alpha Clay Engine cross-platform static library for developing games in C++. Currently supports an entity component system for game objects that are rendered in custom Scenes using OpenGL or OpenGLES got VR. Works on windows and Linux (Tested on Arch and Fedora) and on VR (Tested on Meta Quest 3)

### Third Party Libraries
This libraries uses the following third party libraries:

- assimp
    - For loading 3d models and converting them to renderable components 
- freetype
    - For loading and rendering fonts and text
- GLEW
    - For managing and using OpenGL extensions
- GLFW
    - For creating windows, receiving input, and managing OpenGL contexts, 
- GLM
    - For vector/matrix/quaternion and other graphics related math
- googletest
    - for unit test
- imgui
    - For prototype GUI
- OpenAL
    - For playing audio
- sndfile
    - For converting audio files into audio data useable by OpenAL
- SOIL
    - For loading/saving image files such as pngs or bmp and converting image files into pixel data
- OpenXR-SDK-Source
    - For using this engine on VR platforms

All the libraries are included in `./thirdparty` directory as submodules.

# Instructions 
The Clay Static library can be manually built with the following commands:

- Initialize all third party submodules
    - `git submodule update --init --recursive`  

- To build (outputs to `/build` directory). `-DCLAY_BUILD_TESTS=ON` is optional:
    - `cmake -S . -B build -DCLAY_BUILD_TESTS=ON`
    - `cmake --build ./build/`

- Run test (if build with -DCLAY_BUILD_TESTS=ON):
    - `./build/test/Debug/ClayEngineTest.exe`

Alternatively, in your CMakeLists.txt, the library can be added simply with the following changes and allow Cmake to do all the building and linking
```cmake
set(CLAY_PLATFORM_VR ON CACHE BOOL "Set Platform to VR" FORCE) # If Building for VR
set(CLAY_ENABLE_OPENGL_ES ON CACHE BOOL Enable OpenGLES" FORCE) # If using OpenGLES

set(CLAY_PLATFORM_DESKTOP ON CACHE BOOL "Set Platform to Desktop" FORCE) # If Building for Desktop (Windows or linux)
set(CLAY_ENABLE_OPENGL ON CACHE BOOL "Enable OpenGL" FORCE) # If using OpenGL

# Add ClayEngine
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/clay)
```

A desktop demo project for using this library can be seen [here](https://github.com/VadimEngine/ClayEngineDemo)

A VR Meta Quest demo project for using this library can be seen [here](https://github.com/VadimEngine/ClayEngineDemoVR)
