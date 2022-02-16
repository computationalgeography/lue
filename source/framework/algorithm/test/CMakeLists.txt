add_library(lue_framework_algorithm_test
    flow_accumulation.cpp
)
target_link_libraries(lue_framework_algorithm_test
    PUBLIC
        lue::framework_algorithm
        lue::framework_test
)

set(scope lue_framework_algorithm)

set(local_operation_names
    add
    array_like
    array_partition_id
    atan
    atan2
    cast
    close_to
    copy
    cos
    create_partitioned_array
    divide
    equal_to
    fill
    greater_than
    greater_than_equal_to
    iterate_per_element
    less_than
    less_than_equal_to
    log
    multiply
    not_equal_to
    pow
    sin
    sqrt
    subtract
    valid
    where
)
set(flow_direction_operation_names
    accu
    accu_threshold
    inflow_count
)
set(names
    ${local_operation_names}
    ${flow_direction_operation_names}
    accu_fraction
    all
    any
    array_partition_io
    communicator_array
    convolve
    d8_flow_direction
    downstream
    downstream_distance
    focal_max
    focal_mean
    focal_operation
    focal_sum
    inter_partition_stream
    locality_id
    logical_and
    logical_inclusive_or
    logical_not
    mesh_grid
    maximum
    minimum
    none
    partition_count_unique
    range
    slope
    sum
    timestamp
    uniform
    unique_id
    unique
    upstream
    zonal_sum
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_include_directories(${test_name} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS}
    )

    target_link_libraries(${test_name}
        PRIVATE
            lue_framework_algorithm_test
            lue::framework_algorithm
            lue::framework_test
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND
            ${Python3_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${test_name}>"
    )

    set_tests_properties(
        ${test_name}
        PROPERTIES
            # On Windows, add the path to the HPX dlls. Only if HPX::hpx target is defined.
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<AND:$<PLATFORM_ID:Windows>,$<TARGET_EXISTS:HPX::hpx>>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
    )
endforeach()

foreach(name ${local_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation)
endforeach()

foreach(name ${flow_direction_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_flow_direction_operation)
endforeach()

set(names
    kernel
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_include_directories(${test_name} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS}
    )

    target_link_libraries(${test_name}
        PRIVATE
            lue_framework_algorithm_test
            lue::framework_algorithm
            lue::framework_test
            Boost::unit_test_framework
    )

    add_test(NAME ${test_name}
        COMMAND ${test_name}
    )

    set_tests_properties(
        ${test_name}
        PROPERTIES
            # On Windows, add the path to the HPX dlls. Only if HPX::hpx target is defined.
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<AND:$<PLATFORM_ID:Windows>,$<TARGET_EXISTS:HPX::hpx>>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
    )
endforeach()
