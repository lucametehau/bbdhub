cmake_minimum_required(VERSION 3.15...3.31)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0  # Specify a version tag
)
FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)


add_executable(tests
        board_test.cpp
        fen_test.cpp
)

target_link_libraries(tests PRIVATE
        GTest::gtest_main
        GTest::gmock
        ${YOUR_PROJECT_LIBRARY}
)

target_include_directories(tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(tests)