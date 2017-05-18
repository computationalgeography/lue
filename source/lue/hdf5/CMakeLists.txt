add_test_conditionally(test)

set(SOURCES
    attribute
    attributes
    chunk
    dataset
    dataspace
    datatype
    file
    group
    identifier
    link
    object_info
    property_list
    shape
)
add_library(lue_hdf5_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_hdf5_objects>
    PARENT_SCOPE
)


file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
)
file(RELATIVE_PATH destination
    ${LUE_SOURCE_PREFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/configure.h
        ${SOURCES}
    DESTINATION ${destination}
    COMPONENT python
)
