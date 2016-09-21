set(SOURCES
    attributes
    dataset
    dataspace
    datatype
    file
    group
    identifier
)
add_library(lue_cxx_api_hdf5_objects OBJECT
    ${SOURCES}
)
file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
)
install(
    FILES
        ${SOURCES}
    DESTINATION _lue/lue/cxx_api/hdf5
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
