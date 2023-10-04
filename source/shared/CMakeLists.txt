add_test_conditionally(test)

configure_file(
    include/lue/configure.hpp.in
    include/lue/configure.hpp
)

configure_file(
    src/git.cpp.in
    src/git.cpp
)

configure_file(
    src/version.cpp.in
    src/version.cpp
)

add_library(lue_shared STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/src/git.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/src/version.cpp
)

add_library(lue::shared ALIAS lue_shared)

target_include_directories(lue_shared
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_shared
    PUBLIC
        docopt
        fmt::fmt
)

lue_install_libraries(TARGETS lue_shared)
