cmake_minimum_required(VERSION 3.20)
project(LimitOrderBook)

set(CMAKE_CXX_STANDARD 20)

# Build flag set to optimize for speed
add_compile_options("-O3" "-march=native")

# Tests (optional: enable with -DENABLE_TESTS=ON)
option(ENABLE_TESTS "Build tests" ON)

add_subdirectory(engine)            # LOB matching engine
add_subdirectory(simulation)        # Simulation Engine
#add_subdirectory(visualization)     # LOB Visualization
#add_subdirectory(scripts)           # Parameter Estimation
#add_subdirectory(ctl)               # CLI

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()