add_executable(lob_cli
        src/main.cpp
        src/AsciiRenderer.cpp
        src/CliApp.cpp
)

target_include_directories(lob_cli
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lob_cli
        PRIVATE
        lob_engine
        simulation
)