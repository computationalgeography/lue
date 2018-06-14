add_unit_tests(
    SCOPE lue_cxx
    NAMES

        array/same_shape/value_test
        array/same_shape/constant_shape/value_test
        array/same_shape/variable_shape/value_test

        array/different_shape/value_test
        array/different_shape/constant_shape/value_test
        array/different_shape/variable_shape/value_test

        info/identity/active_id_test
        info/identity/active_set_index_test
        info/identity/active_object_index_test

        # info/...
        # object/...

    LINK_LIBRARIES
        lue::test
)
