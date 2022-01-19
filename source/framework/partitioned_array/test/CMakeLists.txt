add_hpx_unit_tests(
    SCOPE lue_framework_partitioned_array
    NAMES
        array_partition_test.cpp
        partitioned_array_test.cpp
    LINK_LIBRARIES
        lue::framework_partitioned_array
        lue::framework_test
)
