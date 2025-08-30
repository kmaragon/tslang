
set(SOURCES
        tsccore/regex/alternative.cpp
        tsccore/regex/assertion.cpp
        tsccore/regex/atom.cpp
        tsccore/regex/character_class.cpp
        tsccore/regex/disjunction.cpp
        tsccore/regex/error.cpp
        tsccore/regex/group.cpp
        tsccore/regex/quantifier.cpp
        tsccore/regex/regular_expression.cpp
        tsccore/regex/scan_regex.cpp
        tsccore/utf8.cpp
        tsccore/json.cpp
        tsccore/xml.cpp
)

set(HEADERS
        tsccore/regex/alternative.hpp
        tsccore/regex/assertion.hpp
        tsccore/regex/atom.hpp
        tsccore/regex/character_class.hpp
        tsccore/regex/disjunction.hpp
        tsccore/regex/error.hpp
        tsccore/regex/group.hpp
        tsccore/regex/quantifier.hpp
        tsccore/regex/regular_expression.hpp
        tsccore/regex/scan_regex.hpp
        tsccore/regex/scanner.hpp
        tsccore/utf8.hpp
        tsccore/json.hpp
        tsccore/xml.hpp
)

add_library(tsccore ${SOURCES})
target_include_directories(tsccore INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

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
            DESTINATION "include/$install_location"
            COMPONENT "tsclex")

endforeach ()