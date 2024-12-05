cmake_minimum_required(VERSION 3.15...3.31)

add_executable(tests
        board_test.cpp
)

target_include_directories(tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME Tests COMMAND tests)

set_tests_properties(Tests PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)