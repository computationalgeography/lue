set(SOURCES
    time/omnipresent/property_set
    time/omnipresent/constant_shape/property
    # time/omnipresent/constant_shape/property_set
    time/property
    time/property_set
)
add_library(lue_cxx_api_data_type_objects OBJECT
    ${SOURCES}
)
