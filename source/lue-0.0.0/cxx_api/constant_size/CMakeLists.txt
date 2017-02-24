set(SOURCES
    domain
    property
    property_set
    space_domain
    time_domain

    item

    time/omnipresent/domain
    time/omnipresent/item
    time/omnipresent/property
    time/omnipresent/property_set
    time/omnipresent/space_box
    time/omnipresent/space_box_domain
    time/omnipresent/space_domain
    time/omnipresent/different_shape/item
    time/omnipresent/different_shape/property
    time/omnipresent/same_shape/item
    time/omnipresent/same_shape/property

    time/shared/property_set
    time/shared/time_domain
    time/shared/time_period
    time/shared/time_period_domain
)
add_library(lue_cxx_api_constant_size_objects OBJECT
    ${SOURCES}
)
