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
    abs
    acos
    add
    array_like
    array_partition_id
    asin
    atan
    atan2
    cast
    close_to
    copy
    cos
    create_partitioned_array
    divide
    equal_to
    exp
    fill
    greater_than
    greater_than_equal_to
    iterate_per_element
    less_than
    less_than_equal_to
    log
    logical_not
    multiply
    not_equal_to
    pow
    sin
    sqrt
    subtract
    tan
    valid
    where
)

set(focal_operation_names
    aspect
    focal_sum
    slope
)

set(flow_direction_operation_names
    accu
)

if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    # TODO Port these to macOS/M1
    list(APPEND flow_direction_operation_names
        accu_threshold
        inflow_count
    )
endif()

set(names
    ${local_operation_names}
    ${focal_operation_names}
    ${flow_direction_operation_names}
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
    inter_partition_stream
    locality_id
    logical_and
    logical_inclusive_or
    mesh_grid
    maximum
    minimum
    none
    partition_count_unique
    range
    sum
    timestamp
    uniform
    unique_id
    unique
    upstream
    zonal_sum
)

if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    # TODO Port these to macOS/M1
    list(APPEND names
        accu_fraction
    )
endif()

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

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

    if(WIN32)
        # Ensure required DLLs can be found at runtime
        set_property(
            TEST
                ${test_name}
            APPEND
            PROPERTY
                ENVIRONMENT_MODIFICATION
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:HPX::component_storage_component>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_partitioned_array>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_local_operation>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_focal_operation>
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:lue::framework_flow_direction_operation>
        )
    endif()
endforeach()

foreach(name ${local_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation)
endforeach()

foreach(name ${focal_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_focal_operation)
endforeach()

foreach(name ${flow_direction_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_flow_direction_operation)
endforeach()

set(names
    kernel
    out_of_range
)

foreach(name ${names})

    set(module_name ${name}_test)
    set(test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

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

    if(WIN32)
        # Ensure required DLLs can be found at runtime
        set_property(
            TEST
                ${test_name}
            APPEND
            PROPERTY
                ENVIRONMENT_MODIFICATION
                    PATH=path_list_prepend:$<TARGET_FILE_DIR:HPX::component_storage_component>
        )
    endif()
endforeach()
