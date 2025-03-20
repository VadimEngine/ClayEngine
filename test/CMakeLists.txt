# Add the executable for tests
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(ClayEngineTest ${TEST_SOURCES})

set(CLAY_PLATFORM_DESKTOP ON CACHE BOOL "Set Platform to Desktop" FORCE)
set(CLAY_ENABLE_OPENGL ON CACHE BOOL "Enable OpenGL" FORCE)

target_compile_definitions(ClayEngineTest PRIVATE TEST_RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res")

# Link libraries
target_link_libraries(ClayEngineTest
    PRIVATE
    ClayEngine
    gtest
    gtest_main
)

# Add the test executable to CTest
add_test(NAME ClayEngineTest COMMAND ClayEngineTest)