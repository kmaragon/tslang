include(conan)

message(STATUS "${CMAKE_${LANGUAGE}_COMPILER_ID}")
conan_cmake_configure(
        REQUIRES catch2/3.1.0
        GENERATORS cmake_find_package_multi
)

conan_cmake_install(
        PATH_OR_REFERENCE .
        BUILD missing
        SETTINGS ${CONAN_CMAKE_SETTINGS}
        OUTPUT_FOLDER "${CMAKE_BINARY_DIR}/conan_modules")

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}")

message(STATUS "${CMAKE_MODULE_PATH}")
find_package(Catch2 CONFIG REQUIRED)

set(TESTS
        fake_source.cpp
        lexer_tests.cpp
)

add_executable(tsclex.test ${TESTS})
target_link_libraries(
        tsclex.test
        tsclex
        Catch2::Catch2WithMain)