add_library(lob_engine STATIC
        src/Order.cpp
        src/OrderBook.cpp
        src/OrderController.cpp
)

# Make <lob/Order.hpp> and <lob/OrderBook.hpp> available to dependents
target_include_directories(lob_engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# explicitly publish public headers as a FILE_SET
target_sources(lob_engine PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/engine/Order.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/engine/OrderBook.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/engine/OrderController.hpp
)