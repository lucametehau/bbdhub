cmake_minimum_required(VERSION 3.15...3.31)

project(bbd VERSION 1.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

############################################################################
# setting up formatting
file(GLOB_RECURSE ALL_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/src/*.h
        ${PROJECT_SOURCE_DIR}/tests/*.cpp
        ${PROJECT_SOURCE_DIR}/tests/*.h
)

add_custom_target(format-check
        COMMAND ${CLANG_FORMAT} -style=Microsoft -n -Werror ${ALL_SOURCE_FILES}
        COMMENT "Checking code formatting"
        VERBATIM
)

add_custom_target(format-apply
        COMMAND ${CLANG_FORMAT} -style=Microsoft -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting the code"
        VERBATIM
)
############################################################################

set(SOURCES
    src/main.cpp
)

add_executable(bbd ${SOURCES})

target_compile_options(bbd PRIVATE
    -Wall
    -Werror
    -g
)

############################################################################
# include format checking
add_dependencies(bbd format-check)
add_dependencies(format-check format-apply)

############################################################################
# could be tested with "ctest"
enable_testing()
add_subdirectory(tests)

add_custom_target(run-tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMENT "Testing the project"
        VERBATIM
)

get_property(TEST_TARGETS DIRECTORY tests PROPERTY BUILDSYSTEM_TARGETS)
if(TEST_TARGETS)
    add_dependencies(run-tests ${TEST_TARGETS})
endif()

add_dependencies(bbd run-tests)
############################################################################


