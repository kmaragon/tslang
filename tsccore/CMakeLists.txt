
set(SOURCES
        tsccore/utf8.cpp
        tsccore/json.cpp
        )

set(HEADERS
        tsccore/utf8.hpp
        tsccore/json.hpp
        )

add_library(tsccore ${SOURCES})
target_include_directories(tsccore INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

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