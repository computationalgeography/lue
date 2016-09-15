set(SOURCES
    item
    omnipresent/item
    omnipresent/constant_shape/item

    property_set
    omnipresent/property_set
)
add_library(lue_cxx_api_time_objects OBJECT
    ${SOURCES}
)
