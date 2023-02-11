
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake"
            TLS_VERIFY ON)
endif()
include(${CMAKE_BINARY_DIR}/conan.cmake)

set(ENV{CXX} "${CMAKE_CXX_COMPILER}")
set(ENV{CC} "${CMAKE_C_COMPILER}")

conan_cmake_autodetect(CONAN_CMAKE_SETTINGS
        BUILD missing
        BUILD_TYPE ${CMAKE_BUILD_TYPE})
