set(names
    array
    array_visitor
    configuration_entry
    domain_decomposition
    hilbert_curve
    hyperslab
    index_util
    linear_curve
    math
    shape
    span
)

add_unit_tests(
    SCOPE lue_framework_core
    NAMES ${names}
    LIBRARIES
        lue::framework_test
    TARGETS
        test_names
)
