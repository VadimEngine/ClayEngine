cmake_minimum_required(VERSION 3.12)

# Set the project name
project(ClayEngine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BUILD_MODE "x64" CACHE STRING "Build mode: x64 or Win32")

# Define paths to third-party libraries
set(GLEW_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/glew-cmake/out/build/glew/lib/Release/libglew32.lib) # OpenGL API
set(GLFW_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/build/src/Debug/glfw3.lib) # OpenGL window
set(SOIL_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/soil/build/Debug/soil.lib) # Image loading
set(OPENAL_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/openal-soft/build/Debug/OpenAL32.lib) # Audio
set(AL_EXCOMMON_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/openal-soft/build/Debug/alsoft.excommon.lib) # Audio
set(ASSIMP_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/assimp/lib/Debug/assimp-vc142-mtd.lib) # 3d model loading
set(ZLIB_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/assimp/contrib/zlib/Debug/zlibstaticd.lib) # 3d model loading
set(SNDFILE_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/libsndfile/build/Debug/sndfile.lib) # Audio file loading
set(FREETYPE_LIB ${CMAKE_SOURCE_DIR}/ThirdParty/freetype/build/Debug/freetyped.lib) # Font/Text Rendering

set(OPENGL_LIB opengl32.lib) # OpenGL
set(WINMM_LIB winmm.lib) # Windows Mulitmedia
set(AVRT_LIB avrt.lib) # Windows Multimedia Thread API

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add imgui
list(APPEND SRC_FILES
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp
)

# Define the library target
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}
    ${GLEW_LIB}
    ${GLFW_LIB}
    ${SOIL_LIB}
    ${OPENGL_LIB}
    ${ASSIMP_LIB}
    ${ZLIB_LIB}
    ${AL_EXCOMMON_LIB}
    ${OPENAL_LIB}
    ${WINMM_LIB}
    ${SNDFILE_LIB}
    ${FREETYPE_LIB}
    ${AVRT_LIB}
)
# Update the static linker flags
set(
    CMAKE_STATIC_LINKER_FLAGS 
    "${CMAKE_STATIC_LINKER_FLAGS} ${GLEW_LIB} ${GLFW_LIB} ${SOIL_LIB} ${OPENGL_LIB} ${ASSIMP_LIB} ${ZLIB_LIB} ${AL_EXCOMMON_LIB} ${OPENAL_LIB} ${WINMM_LIB} ${SNDFILE_LIB} ${FREETYPE_LIB} ${AVRT_LIB}"
)

# Include directories for public headers
target_include_directories(${PROJECT_NAME} PUBLIC
    # source code
    ${CMAKE_SOURCE_DIR}/include
    # third party
    ${CMAKE_SOURCE_DIR}/ThirdParty/assimp/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/freetype/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glm
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends
    ${CMAKE_SOURCE_DIR}/ThirdParty/openal-soft/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/libsndfile/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/soil/inc/SOIL
)

if (BUILD_TESTS)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    # Build test
    add_subdirectory(${CMAKE_SOURCE_DIR}/ThirdParty/googletest)
    add_subdirectory(test)
endif()