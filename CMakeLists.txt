# ClayEngine CMakeLists.txt
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(ClayEngine)
# For some reason, uncommenting these options breaks VR build
# java.io.FileNotFoundException: /apex/com.meta.xr/priv-app/VrDriver/VrDriver.apk (No such file or directory)
#option(CLAY_PLATFORM_VR "Set Platform to VR" OFF)
#option(CLAY_PLATFORM_DESKTOP "Set Platform to Desktop" OFF)
#option(CLAY_ENABLE_OPENGL "Enable OpenGL support" OFF)
#option(CLAY_ENABLE_OPENGL_ES "Enable OpenGL ES support" OFF)
#option(CLAY_BUILD_TESTS "Enable building Clay Unit tests" OFF)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Assimp, sndfile, glfw
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
set(BUILD_TESTING OFF CACHE BOOL "build tests" FORCE)
# OpenXR
set(BUILD_TESTS OFF CACHE INTERNAL "Disable OpenXR tests" FORCE)
set(BUILD_API_LAYERS ON CACHE INTERNAL "Enable OpenXR API layers" FORCE)

# Add third party directories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/freetype)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libsndfile)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/openal-soft)
if(CLAY_PLATFORM_VR)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/OpenXR-SDK-Source)
endif()
if(CLAY_PLATFORM_DESKTOP)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew-cmake)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/soil)
endif()

# Collect all source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Add imgui source files
list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
)
if(CLAY_PLATFORM_VR)
    list(APPEND SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_android.cpp
    )
endif()
if(CLAY_PLATFORM_DESKTOP)
    list(APPEND SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_glfw.cpp
    )
endif()

# Define the library target
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
if(CLAY_PLATFORM_VR)
    # Android-specific imports
    include(AndroidNdkModules)
    android_ndk_import_module_native_app_glue()
endif()

# Include directories for public headers
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends
)

if(CLAY_PLATFORM_VR)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue
    )
endif()
if(CLAY_PLATFORM_DESKTOP)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/soil/inc/SOIL
    )
endif()

# Define common libraries
set(CLAY_COMMON_LIBS
    freetype
    sndfile
    OpenAL
    assimp
)

if(CLAY_PLATFORM_VR)
    set(CLAY_PLATFORM_LIBS
        android
        EGL
        GLESv3
        log
        native_app_glue
        jnigraphics
        openxr_loader
    )
endif()
if (CLAY_PLATFORM_DESKTOP)
    # Common desktop libraries
    list(APPEND CLAY_PLATFORM_LIBS
        soil
        libglew_static
        glfw
    )

    if (WIN32)
        # Windows
        list(APPEND CLAY_PLATFORM_LIBS
            opengl32
            winmm
            avrt
        )
    elseif (UNIX)
        # Unix
        list(APPEND CLAY_PLATFORM_LIBS
            GL
            pthread
            X11
        )
    endif()
endif()

# Link platform-specific and common libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${CLAY_COMMON_LIBS} ${CLAY_PLATFORM_LIBS})

# Define the DEFAULT_RESOURCE_PATH macro
target_compile_definitions(${PROJECT_NAME} PRIVATE DEFAULT_CLAY_RESOURCE_PATH="${CMAKE_SOURCE_DIR}/res")
if(CLAY_PLATFORM_VR)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CLAY_PLATFORM_VR)
endif()
if(CLAY_PLATFORM_DESKTOP)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CLAY_PLATFORM_DESKTOP)
endif()
if(CLAY_ENABLE_OPENGL)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CLAY_ENABLE_OPENGL)
endif()
if(CLAY_ENABLE_OPENGL_ES)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CLAY_ENABLE_OPENGL_ES)
endif()

if (CLAY_BUILD_TESTS)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    # Add googletest as a subdirectory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest gtest_build)
    # Add the test directory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()