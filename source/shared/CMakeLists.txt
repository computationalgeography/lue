add_test_conditionally(test)

configure_file(
    include/lue/configure.hpp.in
    include/lue/configure.hpp
)

add_library(lue_shared INTERFACE)
add_library(lue::shared ALIAS lue_shared)

target_include_directories(lue_shared
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_shared
    INTERFACE
        fmt::fmt
        lue::compile_options
)

lue_install_libraries(lue_shared)
