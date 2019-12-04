add_unit_tests(
    SCOPE lue_framework_algorithm
    # Tests that use HPX must separate UTF arguments from HPX arguments
    UTF_ARGUMENTS_SEPARATOR "--"
    NAMES
        add_test
        all_test
        cast_test
        convolve_test
        copy_test
        equal_to_test
        fill_test
        focal_max_test
        greater_equal_test
        iterate_per_element_test
        kernel_test
        less_equal_test
        less_test
        multiply_test
        none_test
        range_test
        sum_test
        uniform_test
        unique_id_test
        unique_test
    LINK_LIBRARIES
        lue::framework_algorithm
        lue::framework_test
)

# Targets that depend on HPX
hpx_setup_target(lue_framework_algorithm_add_test)
hpx_setup_target(lue_framework_algorithm_all_test)
hpx_setup_target(lue_framework_algorithm_cast_test)
hpx_setup_target(lue_framework_algorithm_convolve_test)
hpx_setup_target(lue_framework_algorithm_copy_test)
hpx_setup_target(lue_framework_algorithm_equal_to_test)
hpx_setup_target(lue_framework_algorithm_fill_test)
hpx_setup_target(lue_framework_algorithm_focal_max_test)
hpx_setup_target(lue_framework_algorithm_greater_equal_test)
hpx_setup_target(lue_framework_algorithm_iterate_per_element_test)
hpx_setup_target(lue_framework_algorithm_less_equal_test)
hpx_setup_target(lue_framework_algorithm_less_test)
hpx_setup_target(lue_framework_algorithm_multiply_test)
hpx_setup_target(lue_framework_algorithm_none_test)
hpx_setup_target(lue_framework_algorithm_range_test)
hpx_setup_target(lue_framework_algorithm_sum_test)
hpx_setup_target(lue_framework_algorithm_uniform_test)
hpx_setup_target(lue_framework_algorithm_unique_id_test)
hpx_setup_target(lue_framework_algorithm_unique_test)
