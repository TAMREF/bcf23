cmake_minimum_required(VERSION 3.12)

project(bcf23 CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(bcf23
    src/main.cc
)

find_package(Catch2 3 REQUIRED)

# Collect all test source files
file(GLOB TEST_SOURCES "src/*_test.cc")

# Create the executable target
add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# These tests need their own main
# add_executable(custom-main-tests test.cpp test-main.cpp)
# target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)