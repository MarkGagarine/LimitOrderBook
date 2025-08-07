add_executable(test_runner main.cpp)
target_link_libraries(test_runner lob_core)
add_test(NAME core_test COMMAND test_runner)

add_executable(test_order_runner test_order.cpp)
target_link_libraries(test_order_runner lob_core)
add_test(NAME order_test COMMAND test_order_runner)

add_executable(test_orderbook_runner test_orderbook.cpp)
target_link_libraries(test_orderbook_runner lob_core)
add_test(NAME orderbook_test COMMAND test_orderbook_runner)
