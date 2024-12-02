cmake_minimum_required(VERSION 3.15...3.31)

project(bbd LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(SOURCES
    src/main.cpp
)

add_executable(bbd ${SOURCES})

target_compile_options(bbd PRIVATE
    -Wall
    -Werror
    -g
)




