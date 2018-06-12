add_test_conditionally(test)

add_library(lue_hl STATIC
    src/raster
    src/raster_discretization
    src/raster_domain
    src/raster_stack
    src/raster_stack_discretization
    src/raster_stack_domain
    # src/scalar
    src/time_series
    src/time_series_discretization
    src/time_series_domain
)

add_library(lue::hl ALIAS lue_hl)

target_include_directories(lue_hl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_hl
    PUBLIC
        lue::lue
)

install(
    TARGETS
        lue_hl
    EXPORT
        lue_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include
    DESTINATION .
)
