add_test_conditionally(test)

add_library(lue_translate_lib STATIC
    src/stack_name.cpp
    src/translate.cpp

    src/command/export.cpp
    src/command/import.cpp

    # src/format/csv.cpp
    src/format/dot.cpp
    src/format/gdal.cpp
    src/format/gdal_block.cpp
    src/format/gdal_raster.cpp
    src/format/gdal_vector.cpp
    # src/format/gdal_stack.cpp
    # src/format/gdal_stacks.cpp
    # src/format/geo_eas.cpp
    src/format/json.cpp
    src/format/lue.cpp
    src/format/netcdf4.cpp
    src/format/raster_discretization.cpp
    src/format/raster_domain.cpp
    # src/format/vtk.cpp
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
        lue::data_model::netcdf4
    PUBLIC
        lue::utility
        GDAL::GDAL
)

add_executable(lue_translate
    src/main.cpp
)

target_link_libraries(lue_translate
    PRIVATE
        lue::translate
)

install(
    TARGETS lue_translate
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
