

set(HEADERS
        tscctypes/error.hpp
)

add_library(tscctypes ${SOURCES} INTERFACE)
target_include_directories(tscctypes INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

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
            COMPONENT "tscc")

endforeach ()