cmake_minimum_required(VERSION 3.0)

project(3d)

enable_testing()

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
