cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(takehome_tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

add_executable(tetris src/tetris.cpp)
add_executable(tetris_interactive src/interactive.cpp)
add_executable(tetris_tests src/tests.cpp)
