add_test_conditionally(test)

add_library(lue_translate_lib STATIC
    src/stack_name
    src/translate

    src/command/export
    src/command/import

    # src/format/csv
    src/format/dot
    src/format/gdal
    src/format/gdal_block
    src/format/gdal_raster
    src/format/gdal_vector
    # src/format/gdal_stack
    # src/format/gdal_stacks
    # src/format/geo_eas
    src/format/json
    src/format/lue
    src/format/raster_discretization
    src/format/raster_domain
    # src/format/vtk
)

add_library(lue::translate ALIAS lue_translate_lib)

target_include_directories(lue_translate_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


target_link_libraries(lue_translate_lib
    PRIVATE
        lue::data_model_hl
    PUBLIC
        lue::utility
        GDAL::GDAL
)

add_executable(lue_translate
    src/main
)

target_link_libraries(lue_translate
    PRIVATE
        lue::translate
)

install(
    TARGETS lue_translate
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
