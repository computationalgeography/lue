set(names
    no_data_test.cpp
)

add_unit_tests(
    SCOPE lue_shared
    NAMES ${names}
    LINK_LIBRARIES
        lue::shared
)
