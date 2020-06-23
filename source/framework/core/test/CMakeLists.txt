add_unit_tests(
    SCOPE lue_framework_core
    NAMES
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
    LINK_LIBRARIES
        lue::framework_core
        lue::framework_test
)

add_hpx_unit_tests(
    SCOPE lue_framework_core
    NAMES
        array_partition_data_test
        array_partition_test
        partitioned_array_test
    LINK_LIBRARIES
        lue::framework_core
        lue::framework_test
)
