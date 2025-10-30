set(FloatingPointElements ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})
list(TRANSFORM FloatingPointElements PREPEND "\"")
list(TRANSFORM FloatingPointElements APPEND "\"")
list(JOIN FloatingPointElements ", " FloatingPointElements)

set(UnsignedIntegralElements ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS})
list(TRANSFORM UnsignedIntegralElements PREPEND "\"")
list(TRANSFORM UnsignedIntegralElements APPEND "\"")
list(JOIN UnsignedIntegralElements ", " UnsignedIntegralElements)

generate_template_instantiation(
    INPUT_PATHNAME
        "${CMAKE_CURRENT_SOURCE_DIR}/flow_accumulation.cpp.in"
    OUTPUT_PATHNAME
        "${CMAKE_CURRENT_BINARY_DIR}/flow_accumulation.cpp"
    DICTIONARY
    '{"FloatingPointElements":[${FloatingPointElements}],"UnsignedIntegralElements":[${UnsignedIntegralElements}]}'
)

add_library(lue_framework_algorithm_test
    "${CMAKE_CURRENT_BINARY_DIR}/flow_accumulation.cpp"
)

target_include_directories(lue_framework_algorithm_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(lue_framework_algorithm_test
    PUBLIC
        lue::framework_algorithm
        lue::framework_test
)

set(scope lue_framework_algorithm)
set(focal_operation_names
    aspect
    focal_diversity
    focal_high_pass
    focal_majority
    focal_maximum
    focal_mean
    focal_minimum
    focal_sum
    slope
)

set(local_operation_names
    abs
    acos
    add
    array_like
    asin
    atan
    atan2
    cast
    ceil
    cell_index
    close_to
    copy
    cos
    create_partitioned_array
    divide
    equal_to
    exp
    fill
    floor
    greater_than
    greater_than_equal_to
    less_than
    less_than_equal_to
    log
    log10
    logical_and
    logical_exclusive_or
    logical_inclusive_or
    logical_not
    multiply
    modulus
    negate
    not_equal_to
    pow
    round
    sin
    sqrt
    subtract
    tan
    unique_id
    valid
    where
)
list(APPEND local_operation_names
    array_partition_id
    iterate_per_element
)

set(global_operation_names
    unique
)

set(miscellaneous_operation_names
    normal
    resample
    uniform
)

set(routing_operation_names
    accu
    accu_fraction
    accu_threshold
    decreasing_order
    # TODO https://github.com/computationalgeography/lue/issues/629
    # first_n
    kinematic_wave
)
if(LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS)
    list(APPEND routing_operation_names
        accu_info
        inflow_count
    )
endif()

set(zonal_operation_names
    clump
    zonal_area
    zonal_diversity
    zonal_majority
    zonal_maximum
    zonal_mean
    zonal_minimum
    zonal_normal
    zonal_sum
    zonal_uniform
)

set(names
    ${focal_operation_names}
    ${global_operation_names}
    ${local_operation_names}
    ${miscellaneous_operation_names}
    ${routing_operation_names}
    ${zonal_operation_names}
    all
    any
    array_partition_io
    communicator_array
    convolve
    d8_flow_direction
    downstream
    downstream_distance
    focal_operation
    mesh_grid
    maximum
    minimum
    none
    range
    scalar
    sum
    upstream
    verify_compatible
)
if(LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS)
    list(APPEND names
        locality_id
        partition_count_unique
        timestamp
    )
endif()

foreach(name ${names})
    set(module_name ${name}_test)
    string(REPLACE "/" "_" test_name ${scope}_${module_name})

    add_executable(${test_name} ${module_name}.cpp)

    target_link_libraries(${test_name}
        PRIVATE
            lue_framework_algorithm_test
            lue::framework_algorithm
            lue::framework_global_operation  # unique
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
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_partitioned_array>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_partitioned_route>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_local_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_focal_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_miscellaneous_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_routing_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::framework_zonal_operation>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::gdal>>
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
    )

    append_3rd_party_runtime_lib_dirs(${test_name})
endforeach()

foreach(name ${local_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation
    )
endforeach()

foreach(name ${focal_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_focal_operation
    )
endforeach()

foreach(name ${global_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation
            lue::framework_global_operation
    )
endforeach()

foreach(name ${miscellaneous_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation
            lue::framework_miscellaneous_operation
    )
endforeach()

foreach(name ${zonal_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation
            lue::framework_zonal_operation
    )
endforeach()

foreach(name ${routing_operation_names})
    target_link_libraries(${scope}_${name}_test
        PRIVATE
            lue::framework_local_operation
            lue::framework_miscellaneous_operation
            lue::framework_routing_operation
    )
endforeach()

set(names
    kernel
    out_of_range
)

add_unit_tests(
    SCOPE ${scope}
    NAMES ${names}
    LIBRARIES
        lue_framework_algorithm_test
        lue::framework_algorithm
        lue::framework_test
    TARGETS
        test_names
)

set_property(
    TEST
        ${test_names}
    APPEND
    PROPERTY
        ENVIRONMENT_MODIFICATION
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
)
