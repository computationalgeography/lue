set(SOURCES
    property
    omnipresent/constant_shape/property
    omnipresent/variable_shape/property

    property_set
    omnipresent/property_set
    # omnipresent/constant_shape/property_set
)
add_library(lue_cxx_api_data_type_objects OBJECT
    ${SOURCES}
)
