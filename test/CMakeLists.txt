cmake_minimum_required(VERSION 3.12)

# Set the project name for the test executable
project(ClayEngineTest)

# Specify the C++ standard (if needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for the test project
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/assimp/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/freetype/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glew-cmake/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glm2/glm
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui
    ${CMAKE_SOURCE_DIR}/ThirdParty/imgui/backends
    ${CMAKE_SOURCE_DIR}/ThirdParty/openal-soft/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/libsndfile/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/soil/inc/SOIL
)

# Add the source files for the test executable
file(GLOB_RECURSE TEST_SRC_FILES ${CMAKE_SOURCE_DIR}/test/*.cpp)

# Create the executable for the test
add_executable(${PROJECT_NAME} ${TEST_SRC_FILES})

# Link the ClayEngine library and other required libraries
target_link_libraries(${PROJECT_NAME}
    ClayEngine
    gtest_main
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
)
