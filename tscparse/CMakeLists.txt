
set(SOURCES
        tscparse/ast/import_node.cpp
        tscparse/error/declaration_or_statement_expected.cpp
        tscparse/error/expected_token.cpp
        tscparse/error/unexpected_end_of_text.cpp
        tscparse/state/import/binding_import_states.cpp
        tscparse/state/import/equals_import_states.cpp
        tscparse/state/import/from_clause_states.cpp
        tscparse/state/import/import_attributes_state.cpp
        tscparse/state/import/import_node_builder.cpp
        tscparse/state/import/initial_import_states.cpp
        tscparse/state/import/module_completion_states.cpp
        tscparse/state/import/named_import_state.cpp
        tscparse/state/import/namespace_import_states.cpp
        tscparse/state/import_state.cpp
        tscparse/state/module_scope_state.cpp
        tscparse/state/parser_state.cpp
        tscparse/state/state_result.cpp
        tscparse/error.cpp
        tscparse/parser.cpp
)

set(HEADERS
        tscparse/ast/ast_node.hpp
        tscparse/ast/import_node.hpp
        tscparse/error/declaration_or_statement_expected.hpp
        tscparse/error/expected_token.hpp
        tscparse/error/unexpected_end_of_text.hpp
        tscparse/state/import/binding_import_states.hpp
        tscparse/state/import/equals_import_states.hpp
        tscparse/state/import/from_clause_states.hpp
        tscparse/state/import/import_attributes_state.hpp
        tscparse/state/import/import_node_builder.hpp
        tscparse/state/token_helpers.hpp
        tscparse/state/import/initial_import_states.hpp
        tscparse/state/import/module_completion_states.hpp
        tscparse/state/import/named_import_state.hpp
        tscparse/state/import/namespace_import_states.hpp
        tscparse/state/import_state.hpp
        tscparse/state/module_scope_state.hpp
        tscparse/state/parser_state.hpp
        tscparse/state/state_result.hpp
        tscparse/error.hpp
        tscparse/filtered_token_index.hpp
        tscparse/parser.hpp
        tscparse/trivia_index.hpp
)

add_library(tscparse ${SOURCES})
target_include_directories(tscparse INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tscparse tsccore tsclex)

add_subdirectory(test)

foreach (header_path ${HEADERS})
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
            DESTINATION "include/${install_location}"
            COMPONENT "tscc")

endforeach ()
