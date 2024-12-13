# Add the executable for tests
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(ClayEngineTest ${TEST_SOURCES})

# Link libraries
target_link_libraries(ClayEngineTest
    PRIVATE
    ClayEngine
    gtest
    gtest_main
)

# Add the test executable to CTest
add_test(NAME ClayEngineTest COMMAND ClayEngineTest)