set(SOURCES
    domain
    item
    property
    property_set
    space_domain
    time_domain

    omnipresent/domain
    omnipresent/item
    omnipresent/property_set
    omnipresent/space_box
    omnipresent/space_box_domain
    omnipresent/space_domain
    omnipresent/different_shape/item
    omnipresent/different_shape/property
    omnipresent/same_shape/item
    omnipresent/same_shape/property

    shared/time_domain
    shared/time_period
    shared/time_period_domain

    shared_constant/property_set
)
add_library(lue_cxx_api_time_objects OBJECT
    ${SOURCES}
)
