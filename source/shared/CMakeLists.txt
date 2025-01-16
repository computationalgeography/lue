configure_file(
    include/lue/configure.hpp.in
    include/lue/configure.hpp
)

configure_file(
    source/git.cpp.in
    source/git.cpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
)

add_library(lue_shared STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/source/git.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/source/version.cpp
)

add_library(lue::shared ALIAS lue_shared)

set_target_properties(lue_shared
    PROPERTIES
        EXPORT_NAME shared
)

target_include_directories(lue_shared
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(lue_shared
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/configure.hpp
)

target_link_libraries(lue_shared
    PUBLIC
        fmt::fmt
)

lue_install_development_libraries(
    TARGETS lue_shared
)
