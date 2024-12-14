cmake_minimum_required(VERSION 3.12)

# Set the project name
project(ClayEngine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Assimp, sndfile glfw
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
# assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "If the test suite for Assimp is built in addition to the library." FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Disable this if you want to use assimp as a submodule." FORCE)
# Open AL
set(LIBTYPE STATIC CACHE STRING "Build OpenAL as a static library" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "Build test programs")
set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example program")
# GLEW
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "Build the shared glew library")
set(ONLY_LIBS ON CACHE BOOL "Do not build executables")
# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)
# sndfile
set(BUILD_PROGRAMS OFF CACHE BOOL "Build program" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_TESTING  OFF CACHE BOOL "build tests" FORCE)

# Add third party directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/freetype)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glew-cmake)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/libsndfile)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/openal-soft)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/soil)

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add imgui source files
list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp
)

# Define the library target
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

# Define the DEFAULT_RESOURCE_PATH macro
target_compile_definitions(${PROJECT_NAME} PRIVATE DEFAULT_RESOURCE_PATH="${CMAKE_SOURCE_DIR}/res")

# Define platform-specific libraries
if (WIN32)
    set(CLAY_PLATFORM_LIBS
        opengl32
        winmm
        avrt
    )
elseif (UNIX)
    set(CLAY_PLATFORM_LIBS
        GL         # OpenGL library on Linux
        pthread    # Necessary for threading
        X11        # X Window System
    )
endif()

# Define common libraries
set(CLAY_COMMON_LIBS
    assimp
    freetype
    libglew_static
    glfw
    sndfile
    OpenAL
    soil
)

# Combine platform-specific and common libraries
target_link_libraries(${PROJECT_NAME} ${CLAY_PLATFORM_LIBS} ${CLAY_COMMON_LIBS})

# Include directories for public headers
target_include_directories(${PROJECT_NAME} PUBLIC
    # source code
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # third party
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/soil/inc/SOIL
)

if (CLAY_BUILD_TESTS)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    # Add googletest as a subdirectory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/googletest gtest_build)
    # Add the test directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()