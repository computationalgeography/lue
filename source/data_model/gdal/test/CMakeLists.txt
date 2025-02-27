set(names
    blocks
    dataset
    driver
    raster
    raster_band
    type_traits
    version
)

add_unit_tests(
    SCOPE lue_gdal
    NAMES ${names}
    LIBRARIES
        lue_gdal_static
        lue::shared
    EXPORT_MACRO_BASENAME
        GDAL
    TARGETS
        test_names
)

foreach(test_name ${test_names})
    set_property(
        TEST
            ${test_name}
        APPEND
        PROPERTY
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:GDAL::GDAL>/../bin>
    )
endforeach()
