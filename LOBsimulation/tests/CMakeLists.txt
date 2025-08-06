add_executable(test_runner main.cpp)
target_link_libraries(test_runner lob_core)
add_test(NAME core_test COMMAND test_runner)