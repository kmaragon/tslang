
set(SOURCES
        tsclex/error/expected_command.cpp
        tsclex/error/unexpected_token.cpp
        tsclex/tokens/basic_token.cpp
        tsclex/tokens/comment_token.cpp
        tsclex/tokens/shebang_token.cpp
        tsclex/lexer.cpp
        tsclex/source.cpp
        tsclex/source_location.cpp
        tsclex/token.cpp
        )

set(HEADERS
        tsclex/error/expected_command.hpp
        tsclex/error/unexpected_token.hpp
        tsclex/tokens/basic_token.hpp
        tsclex/tokens/comment_token.hpp
        tsclex/tokens/shebang_token.hpp
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