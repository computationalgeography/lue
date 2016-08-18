set(SOURCES
    attributes
    dataset
    dataspace
    file
    group
    identifier
)
add_library(lue_cxx_api_hdf5_objects OBJECT
    ${SOURCES}
)
