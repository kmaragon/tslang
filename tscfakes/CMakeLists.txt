
set(SOURCES
        fake_source.cpp
)

set(HEADERS
        fake_source.hpp
        test_common.hpp
)

add_library(tscfakes ${SOURCES})
target_include_directories(tscfakes PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(tscfakes PUBLIC tsclex)