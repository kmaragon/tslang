cmake_minimum_required(VERSION 3.23)
project(tscc)

set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(tsccore)
add_subdirectory(tsclex)
