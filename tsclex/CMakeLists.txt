
set(SOURCES
        tsclex/error/expected_command.cpp
        tsclex/error/invalid_character.cpp
        tsclex/error/invalid_identifier.cpp
        tsclex/error/misplaced_shebang.cpp
        tsclex/error/unexpected_token.cpp
        tsclex/tokens/ampersand_eq_token.cpp
        tsclex/tokens/ampersand_token.cpp
        tsclex/tokens/asterisk_eq_token.cpp
        tsclex/tokens/asterisk_token.cpp
        tsclex/tokens/at_token.cpp
        tsclex/tokens/bar_eq_token.cpp
        tsclex/tokens/bar_token.cpp
        tsclex/tokens/basic_token.cpp
        tsclex/tokens/caret_eq_token.cpp
        tsclex/tokens/caret_token.cpp
        tsclex/tokens/close_brace_token.cpp
        tsclex/tokens/close_bracket_token.cpp
        tsclex/tokens/close_paren_token.cpp
        tsclex/tokens/colon_token.cpp
        tsclex/tokens/comma_token.cpp
        tsclex/tokens/comment_token.cpp
        tsclex/tokens/dot_token.cpp
        tsclex/tokens/double_ampersand_eq_token.cpp
        tsclex/tokens/double_ampersand_token.cpp
        tsclex/tokens/double_asterisk_eq_token.cpp
        tsclex/tokens/double_asterisk_token.cpp
        tsclex/tokens/double_bar_eq_token.cpp
        tsclex/tokens/double_bar_token.cpp
        tsclex/tokens/double_eq_token.cpp
        tsclex/tokens/double_greater_eq_token.cpp
        tsclex/tokens/double_greater_token.cpp
        tsclex/tokens/double_less_eq_token.cpp
        tsclex/tokens/double_less_token.cpp
        tsclex/tokens/double_minus_token.cpp
        tsclex/tokens/double_plus_token.cpp
        tsclex/tokens/double_question_eq_token.cpp
        tsclex/tokens/double_question_token.cpp
        tsclex/tokens/eq_greater_token.cpp
        tsclex/tokens/eq_token.cpp
        tsclex/tokens/exclamation_eq_eq_token.cpp
        tsclex/tokens/exclamation_eq_token.cpp
        tsclex/tokens/exclamation_token.cpp
        tsclex/tokens/greater_eq_token.cpp
        tsclex/tokens/greater_token.cpp
        tsclex/tokens/less_eq_token.cpp
        tsclex/tokens/less_token.cpp
        tsclex/tokens/minus_eq_token.cpp
        tsclex/tokens/minus_token.cpp
        tsclex/tokens/newline_token.cpp
        tsclex/tokens/open_brace_token.cpp
        tsclex/tokens/open_bracket_token.cpp
        tsclex/tokens/open_paren_token.cpp
        tsclex/tokens/percent_eq_token.cpp
        tsclex/tokens/percent_token.cpp
        tsclex/tokens/plus_eq_token.cpp
        tsclex/tokens/plus_token.cpp
        tsclex/tokens/question_dot_token.cpp
        tsclex/tokens/question_token.cpp
        tsclex/tokens/semicolon_token.cpp
        tsclex/tokens/slash_eq_token.cpp
        tsclex/tokens/slash_token.cpp
        tsclex/tokens/shebang_token.cpp
        tsclex/tokens/tilde_token.cpp
        tsclex/tokens/triple_dot_token.cpp
        tsclex/tokens/triple_eq_token.cpp
        tsclex/lexer.cpp
        tsclex/source.cpp
        tsclex/source_location.cpp
        tsclex/token.cpp
        )

set(HEADERS
        tsclex/error/expected_command.hpp
        tsclex/error/invalid_character.hpp
        tsclex/error/invalid_identifier.hpp
        tsclex/error/misplaced_shebang.hpp
        tsclex/error/unexpected_token.hpp
        tsclex/tokens/ampersand_eq_token.hpp
        tsclex/tokens/ampersand_token.hpp
        tsclex/tokens/asterisk_eq_token.hpp
        tsclex/tokens/asterisk_token.hpp
        tsclex/tokens/at_token.hpp
        tsclex/tokens/bar_eq_token.hpp
        tsclex/tokens/bar_token.hpp
        tsclex/tokens/basic_token.hpp
        tsclex/tokens/caret_eq_token.hpp
        tsclex/tokens/caret_token.hpp
        tsclex/tokens/close_brace_token.hpp
        tsclex/tokens/close_bracket_token.hpp
        tsclex/tokens/close_paren_token.hpp
        tsclex/tokens/colon_token.hpp
        tsclex/tokens/comma_token.hpp
        tsclex/tokens/comment_token.hpp
        tsclex/tokens/dot_token.hpp
        tsclex/tokens/double_ampersand_eq_token.hpp
        tsclex/tokens/double_ampersand_token.hpp
        tsclex/tokens/double_asterisk_eq_token.hpp
        tsclex/tokens/double_asterisk_token.hpp
        tsclex/tokens/double_bar_eq_token.hpp
        tsclex/tokens/double_bar_token.hpp
        tsclex/tokens/double_eq_token.hpp
        tsclex/tokens/double_greater_eq_token.hpp
        tsclex/tokens/double_greater_token.hpp
        tsclex/tokens/double_less_eq_token.hpp
        tsclex/tokens/double_less_token.hpp
        tsclex/tokens/double_minus_token.hpp
        tsclex/tokens/double_plus_token.hpp
        tsclex/tokens/double_question_eq_token.hpp
        tsclex/tokens/double_question_token.hpp
        tsclex/tokens/eq_greater_token.hpp
        tsclex/tokens/eq_token.hpp
        tsclex/tokens/exclamation_eq_eq_token.hpp
        tsclex/tokens/exclamation_eq_token.hpp
        tsclex/tokens/exclamation_token.hpp
        tsclex/tokens/greater_eq_token.hpp
        tsclex/tokens/greater_token.hpp
        tsclex/tokens/less_eq_token.hpp
        tsclex/tokens/less_token.hpp
        tsclex/tokens/minus_eq_token.hpp
        tsclex/tokens/minus_token.hpp
        tsclex/tokens/newline_token.hpp
        tsclex/tokens/open_brace_token.hpp
        tsclex/tokens/open_bracket_token.hpp
        tsclex/tokens/open_paren_token.hpp
        tsclex/tokens/percent_eq_token.hpp
        tsclex/tokens/percent_token.hpp
        tsclex/tokens/plus_eq_token.hpp
        tsclex/tokens/plus_token.hpp
        tsclex/tokens/question_dot_token.hpp
        tsclex/tokens/question_token.hpp
        tsclex/tokens/semicolon_token.hpp
        tsclex/tokens/slash_eq_token.hpp
        tsclex/tokens/slash_token.hpp
        tsclex/tokens/shebang_token.hpp
        tsclex/tokens/tilde_token.hpp
        tsclex/tokens/triple_dot_token.hpp
        tsclex/tokens/triple_eq_token.hpp
        tsclex/lexer.hpp
        tsclex/source.hpp
        tsclex/source_location.hpp
        tsclex/token.hpp
        )

add_library(tsclex ${SOURCES})
target_include_directories(tsclex INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(test)

foreach(header_path ${HEADERS})
    # Convert the header to its absolute path
    # so we can get the appropriate relative path
    get_filename_component(header_path "${header_path}" REALPATH)

    # Get the install path for the header file
    file(RELATIVE_PATH
            install_location
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${header_path}")

    # install the header into the relative install path
    install(FILES "${header_path}"
            DESTINATION "include/$install_location"
            COMPONENT "tsclex")

endforeach()