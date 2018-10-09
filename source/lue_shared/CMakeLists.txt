configure_file(
    include/lue/configure.hpp.in
    include/lue/configure.hpp
)

add_library(lue_shared INTERFACE)
add_library(lue::shared ALIAS lue_shared)

target_include_directories(lue_shared
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

install(
    TARGETS
        lue_shared
    EXPORT
        lue_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/include
    DESTINATION .
)
