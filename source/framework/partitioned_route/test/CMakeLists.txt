set(scope lue_framework_partitioned_route)
set(names
    serial_route_fragment
)

add_unit_tests(
    SCOPE ${scope}
    NAMES ${names}
    LIBRARIES
        lue::framework_test
        lue::framework_partitioned_route
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
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:${HWLOC_ROOT}/bin>
    )
endforeach()

set(names
    serial_route
    serial_route_partition
)

foreach(name ${names})
    set(module_name ${name}_test)
    string(REPLACE "/" "_" test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue::framework_partitioned_route
            lue::framework_test
            Boost::headers
    )

    add_test(NAME ${test_name}
        COMMAND
            ${Python_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_QUALITY_ASSURANCE_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_QUALITY_ASSURANCE_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_QUALITY_ASSURANCE_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${test_name}>"
    )

    set_property(
        TEST
            ${test_name}
        APPEND
        PROPERTY
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:GDAL::GDAL>/../bin>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:${HWLOC_ROOT}/bin>
    )
endforeach()
