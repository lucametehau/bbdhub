cmake_minimum_required(VERSION 3.15...3.31)

include(FetchContent)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # enables LTO


FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)


add_executable(tests
        #evaluation_test.cpp
        board_test.cpp
        perft_test.cpp
        fen_test.cpp
        search_test.cpp
        hash_calc_test.cpp
        threefold_test.cpp
        incremental_hash_calc_test.cpp
        ../src/board.cpp
        ../src/search.cpp
)

target_compile_options(tests PRIVATE
    -Wall
    -Werror
    -g
    -O3
    --verbose
    -std=c++20
)


target_link_libraries(tests PRIVATE
        GTest::gtest_main
        GTest::gmock
)

target_include_directories(tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

set_tests_properties(${Tests} PROPERTIES TIMEOUT 5)

include(GoogleTest)
gtest_discover_tests(tests)