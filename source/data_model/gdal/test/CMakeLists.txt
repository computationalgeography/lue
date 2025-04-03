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
