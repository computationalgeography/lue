set(SOURCES
    domain
    omnipresent/domain

    omnipresent/space_box

    space_domain
    omnipresent/space_domain
    omnipresent/space_box_domain

    item
    omnipresent/item
    omnipresent/constant_shape/item
    omnipresent/variable_shape/item

    property
    omnipresent/constant_shape/property
    omnipresent/variable_shape/property

    property_set
    omnipresent/property_set
    # omnipresent/constant_shape/property_set
)
add_library(lue_cxx_api_time_objects OBJECT
    ${SOURCES}
)
