
set(SOURCES
        tscparse/error/expected_token.cpp
        tscparse/error.cpp
        tscparse/parser.cpp
)

set(HEADERS
        tscparse/ast/ast_node.hpp
        tscparse/error/expected_token.hpp
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
            COMPONENT "tscparse")

endforeach ()