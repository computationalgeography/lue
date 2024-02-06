set(scope lue_gdal)

set(names
    blocks_test
    dataset_test
    driver_test
    raster_test
    raster_band_test
    type_traits_test
    version_test
)

foreach(name ${names})

    set(test_name ${scope}_${name})

    # set(module_name ${name}_test)
    # set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::gdal
            lue::shared
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )
endforeach()
