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