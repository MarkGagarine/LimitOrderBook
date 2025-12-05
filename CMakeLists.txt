cmake_minimum_required(VERSION 3.23)

project(LimitOrderBook)

# Enforce case sensitive file matching
cmake_policy(SET CMP0141 NEW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build flag set to optimize for speed
add_compile_options(-O3 -march=native)

# Tests (optional: enable with -DENABLE_TESTS=ON)
option(ENABLE_TESTS "Build tests" ON)

add_subdirectory(engine)            # LOB matching engine
add_subdirectory(simulation)        # Simulation Engine
add_subdirectory(cli)               # CLI

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()