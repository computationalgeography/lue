add_unit_tests(
    SCOPE lue_data_model_netcdf4
    NAMES
        file_test.cpp
    LINK_LIBRARIES
        lue::data_model::netcdf4
)
