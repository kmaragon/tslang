find_package(Catch2 CONFIG REQUIRED)

set(TESTS
        parser_tests.cpp
)

add_executable(tscparse.test ${TESTS})
target_link_libraries(
        tscparse.test
        tscparse
        tsclex
        tsccore
        tscfakes
        Catch2::Catch2WithMain)

add_test(tscparse_test tscparse.test)