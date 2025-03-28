cmake_minimum_required(VERSION 3.21)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(conan)

set(CMAKE_CXX_STANDARD 23)

add_compile_options("-Wall" "-Wno-trigraphs")

cmake_minimum_required(VERSION 3.23)
project(tscc)

add_subdirectory(tsclex)
add_subdirectory(tsccore)