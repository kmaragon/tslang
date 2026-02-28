
set(SOURCES
        tscparse/ast/declare_module_node.cpp
        tscparse/ast/import_node.cpp
        tscparse/ast/module_node.cpp
        tscparse/ast/namespace_node.cpp
        tscparse/ast/qualified_name.cpp
        tscparse/ast/source_file_node.cpp
        tscparse/ast/type/array_type_node.cpp
        tscparse/ast/type/intersection_type_node.cpp
        tscparse/ast/type/keyword_type_node.cpp
        tscparse/ast/type/literal_type_node.cpp

        tscparse/ast/type/this_type_node.cpp
        tscparse/ast/type_node.cpp
        tscparse/ast/type/type_parameter_node.cpp
        tscparse/ast/type/type_reference_node.cpp
        tscparse/ast/type/union_type_node.cpp
        tscparse/error/declare_in_ambient_context.cpp
        tscparse/error/declaration_or_statement_expected.cpp
        tscparse/error/expected_token.cpp
        tscparse/error/unexpected_end_of_text.cpp
        tscparse/state/declare/declare_module_header_state.cpp
        tscparse/state/declare/declare_module_state.cpp
        tscparse/state/import/binding_import_states.cpp
        tscparse/state/import/equals_import_states.cpp
        tscparse/state/import/from_clause_states.cpp
        tscparse/state/import/import_attributes_state.cpp
        tscparse/state/import/import_node_builder.cpp
        tscparse/state/import/initial_import_states.cpp
        tscparse/state/import/module_completion_states.cpp
        tscparse/state/import/named_import_state.cpp
        tscparse/state/import/namespace_import_states.cpp
        tscparse/state/namespace/namespace_header_state.cpp
        tscparse/state/type/type_argument_list_state.cpp
        tscparse/state/type/type_expression_state.cpp
        tscparse/state/type/type_header_state.cpp
        tscparse/state/type/type_intersection_state.cpp
        tscparse/state/type/type_operator_state.cpp
        tscparse/state/type/type_paren_state.cpp
        tscparse/state/type/type_parameter_list_state.cpp
        tscparse/state/type/type_parameter_state.cpp
        tscparse/state/type/type_postfix_state.cpp
        tscparse/state/type/type_primary_state.cpp
        tscparse/state/type/type_reference_state.cpp
        tscparse/state/type/type_union_state.cpp
        tscparse/state/declare_state.cpp
        tscparse/state/import_state.cpp
        tscparse/state/module_scope_state.cpp
        tscparse/state/namespace_state.cpp
        tscparse/state/parser_state.cpp
        tscparse/state/state_result.cpp
        tscparse/state/type_state.cpp
        tscparse/error.cpp
        tscparse/parser.cpp
)

set(HEADERS
        tscparse/ast/ast_node.hpp
        tscparse/ast/declare_module_node.hpp
        tscparse/ast/import_node.hpp
        tscparse/ast/lexeme.hpp
        tscparse/ast/lexeme_visitors.ipp
        tscparse/ast/module_node.hpp
        tscparse/ast/namespace_node.hpp
        tscparse/ast/qualified_name.hpp
        tscparse/ast/source_file_node.hpp
        tscparse/ast/type_node.hpp
        tscparse/ast/type/array_type_node.hpp
        tscparse/ast/type/intersection_type_node.hpp
        tscparse/ast/type/keyword_type_node.hpp
        tscparse/ast/type/literal_type_node.hpp

        tscparse/ast/type/this_type_node.hpp
        tscparse/ast/type/type_definition.hpp
        tscparse/ast/type/type_context.hpp
        tscparse/ast/type/type_parameter_node.hpp
        tscparse/ast/type/type_reference_node.hpp
        tscparse/ast/type/union_type_node.hpp
        tscparse/error/declare_in_ambient_context.hpp
        tscparse/error/declaration_or_statement_expected.hpp
        tscparse/error/expected_token.hpp
        tscparse/error/unexpected_end_of_text.hpp
        tscparse/state/import/binding_import_states.hpp
        tscparse/state/import/equals_import_states.hpp
        tscparse/state/import/from_clause_states.hpp
        tscparse/state/import/import_attributes_state.hpp
        tscparse/state/import/import_node_builder.hpp
        tscparse/state/import/initial_import_states.hpp
        tscparse/state/import/module_completion_states.hpp
        tscparse/state/import/named_import_state.hpp
        tscparse/state/import/namespace_import_states.hpp
        tscparse/state/namespace/namespace_header_state.hpp
        tscparse/state/type/type_argument_list_state.hpp
        tscparse/state/type/type_expression_state.hpp
        tscparse/state/type/type_header_state.hpp
        tscparse/state/type/type_intersection_state.hpp
        tscparse/state/type/type_operator_state.hpp
        tscparse/state/type/type_paren_state.hpp
        tscparse/state/type/type_parameter_list_state.hpp
        tscparse/state/type/type_parameter_state.hpp
        tscparse/state/type/type_postfix_state.hpp
        tscparse/state/type/type_primary_state.hpp
        tscparse/state/type/type_reference_state.hpp
        tscparse/state/type/type_union_state.hpp
        tscparse/state/import_state.hpp
        tscparse/state/module_scope_state.hpp
        tscparse/state/module_scope_visitor.hpp
        tscparse/state/namespace_state.hpp
        tscparse/state/parser_state.hpp
        tscparse/state/state_result.hpp
        tscparse/state/token_helpers.hpp
        tscparse/state/type_state.hpp
        tscparse/error.hpp
        tscparse/filtered_token_index.hpp
        tscparse/parser.hpp
        tscparse/parser_observer.hpp
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
