add_library(simulation
        src/Simulation.cpp
)

target_include_directories(simulation
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(simulation
        PRIVATE
        lob_engine
)

