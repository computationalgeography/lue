add_library(lue_shared INTERFACE)
add_library(lue::shared ALIAS lue_shared)

set_target_properties(lue_shared
    PROPERTIES
        EXPORT_NAME shared
)

configure_file(
    include/lue/configure.hpp.in
    include/lue/configure.hpp
)

configure_file(
    include/lue/git.hpp.in
    include/lue/git.hpp
)

configure_file(
    include/lue/version.hpp.in
    include/lue/version.hpp
)

target_sources(lue_shared
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/configure.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/git.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/version.hpp
)

target_link_libraries(lue_shared
    INTERFACE
        lue::compile_features
)

lue_install_development_libraries(
    TARGETS lue_shared
)
