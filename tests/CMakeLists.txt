cmake_minimum_required(VERSION 3.15...3.31)

add_executable(board_tests
        board_test.cpp
        ../src/board.cpp
)

target_include_directories(board_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME "Board Tests" COMMAND board_tests)

set_tests_properties("Board Tests" PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)