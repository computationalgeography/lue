set(names
    array
    array_visitor
    bounding_box
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

set_property(
    TEST
        ${test_names}
    APPEND
    PROPERTY
        ENVIRONMENT_MODIFICATION
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:HPX::component_storage_component>>
)
