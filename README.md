Early Alpha Clay Engine static library for developing games in C++. Currently supports an entity component system for game objects that are rendered in a Scene,

Third party files need to be unzipped to run to work around the 100mb file size limit in github.
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
- openAL
    - For playing audio files
- sndfile
    - For loading audio files
- SOIL
    - For loading/saving images such as pngs or bmp

### Instructions 
Windows PowerShell commands (Only works for x64 due to difficulty getting Assimp to work for x86):

- To build (outputs to `\build` directory). `-DBUILD_TESTS=ON` is optional:
    - `cmake -S . -B build -DBUILD_TESTS=ON`
    - `cmake --build .\build\`
    - Combined with clean: 
        - `cmake -S . -B build ; cmake --build .\build\ --clean-first`
        - OR `.\build-tool.ps1 -Clean`

- Run test:
    - `.\build\test\Debug\ClayEngineTest.exe`