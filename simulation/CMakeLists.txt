add_library(simulation
        src/simulation.cpp
)

target_compile_features(simulation PUBLIC cxx_std_20)

target_link_libraries(simulation PRIVATE lob_engine)

target_include_directories(lob_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)