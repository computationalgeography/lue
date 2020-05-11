set(names
        array_test
        array_partition_definition_test
        array_visitor_test
        configuration_entry_test
        domain_decomposition_test
        hilbert_curve_test
        linear_curve_test
        math_test
        shared_buffer_test
        shape_test
        span_test
)

add_unit_tests(
    SCOPE lue_framework_core
    NAMES
        ${names}
    LINK_LIBRARIES
        lue::framework_core
        lue::framework_test
)

foreach(name ${names})
    set(target_name lue_framework_core_${name})

    hpx_setup_target(${target_name})

    # set_target_properties(${target_name}
    #     PROPERTIES
    #         CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip hpx headers
    # )
endforeach()


# Targets that depend on HPX
set(names
    array_partition_data_test
    array_partition_test
    partitioned_array_test
)

add_unit_tests(
    SCOPE lue_framework_core
    # Tests that use HPX must separate UTF arguments from HPX arguments
    UTF_ARGUMENTS_SEPARATOR "--"
    NAMES
        ${names}
    LINK_LIBRARIES
        lue::framework_core
        lue::framework_test
)

foreach(name ${names})
    set(target_name lue_framework_core_${name})

    hpx_setup_target(${target_name})
endforeach()
