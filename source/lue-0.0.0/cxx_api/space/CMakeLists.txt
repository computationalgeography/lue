set(SOURCES

    space_domain
    stationary/space_domain

    stationary/space_box_domain

    stationary/space_box

)
add_library(lue_cxx_api_space_objects OBJECT
    ${SOURCES}
)
