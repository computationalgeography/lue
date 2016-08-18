set(SOURCES
    hdf5_attribute
    hdf5_dataspace
    hdf5_datatype
    hdf5_file
    hdf5_group
    hdf5_identifier
    hdf5_link
)
add_library(lue_c_api_hdf5_objects OBJECT
    ${SOURCES}
)
file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.c *.h
)
install(
    FILES
        ${SOURCES}
    DESTINATION _lue/lue/c_api/hdf5
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
