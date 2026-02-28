cmake_minimum_required(VERSION 3.21)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(conan)
include(CPackComponent)

set(CMAKE_CXX_STANDARD 23)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)

option(ENABLE_CLANG_TIDY "Run clang-tidy during build" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()

add_compile_options("-Wall" "-Wno-trigraphs")

cmake_minimum_required(VERSION 3.23)
project(tscc)

enable_testing()

# define the base component that is used by both the compiler and the runtime
cpack_add_component(tscbase)

# define the component that includes the compiler toolchain
cpack_add_component(tscc DEPENDS tscbase)

add_subdirectory(tsccore)
add_subdirectory(tscctypes)
add_subdirectory(tsclex)
add_subdirectory(tscparse)
add_subdirectory(tscfakes)

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
    add_subdirectory(docs)
endif()