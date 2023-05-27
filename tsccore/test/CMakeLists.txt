find_package(Catch2 CONFIG REQUIRED)

set(TESTS
    utf8_tests.cpp
        )

add_executable(tsccore.test ${TESTS})
target_link_libraries(
        tsccore.test
        tsccore
        Catch2::Catch2WithMain)