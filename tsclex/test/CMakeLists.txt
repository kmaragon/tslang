find_package(Catch2 CONFIG REQUIRED)

set(TESTS
        fake_source.cpp
        shebang_tests.cpp
        single_line_comment_tests.cpp
        keywords_tests.cpp
        multi_line_comment_tests.cpp
        utf8_handling_tests.cpp
        operators_tests.cpp
        punctuation_tests.cpp
        string_escape_tests.cpp
        comment_edge_cases_tests.cpp
        literals_tests.cpp
        typescript_extensions_tests.cpp
        lambdas_tests.cpp
        template_literals_tests.cpp
        decorators_tests.cpp
        keywords_additional_tests.cpp
        regex_tests.cpp
        jsx_tests.cpp
)

add_executable(tsclex.test ${TESTS})
target_link_libraries(
        tsclex.test
        tsclex
        Catch2::Catch2WithMain)