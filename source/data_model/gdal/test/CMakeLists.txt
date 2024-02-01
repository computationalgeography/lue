set(scope lue_gdal)

set(names
    dataset_test
    driver_test
    raster_band_test
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
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )
endforeach()
