cmake_minimum_required(VERSION 3.0)

project(shadow LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

execute_process(
    COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
    include
    external
)

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(test)
