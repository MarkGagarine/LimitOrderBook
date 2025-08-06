cmake_minimum_required(VERSION 3.20)
project(LOBsimulation)

set(CMAKE_CXX_STANDARD 17)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE CORE_SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

# Create shared library
add_library(lob_core SHARED ${CORE_SOURCES} ${HEADERS})

# Unit Tests (optional: enable with -DENABLE_TESTS=ON)
option(ENABLE_TESTS "Build tests" ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()