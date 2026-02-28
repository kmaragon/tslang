find_package(Python3 COMPONENTS Interpreter REQUIRED)

configure_file(Doxyfile.in Doxyfile @ONLY)

# Target: run parser tests with transition logging, then generate DOT files
add_custom_target(gen-diagrams
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/diagrams
    COMMAND ${CMAKE_COMMAND} -E env
        TSCC_TRANSITION_LOG=${CMAKE_CURRENT_BINARY_DIR}/transitions.log
        $<TARGET_FILE:tscparse.test>
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen_diagrams.py
        --transitions ${CMAKE_CURRENT_BINARY_DIR}/transitions.log
        --config ${CMAKE_CURRENT_SOURCE_DIR}/diagram_config.yaml
        --output-dir ${CMAKE_CURRENT_BINARY_DIR}/diagrams
    DEPENDS $<TARGET_FILE:tscparse.test> diagram_config.yaml
    COMMENT "Generating parser state diagrams from test transitions"
    VERBATIM
)

# Target: build full HTML docs (depends on fresh diagrams)
add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS gen-diagrams
    COMMENT "Generating documentation with Doxygen"
    VERBATIM
)
