add_test_conditionally(test)
set(SOURCES
    attribute
    attributes
    dataset
    dataspace
    datatype
    file
    group
    identifier
    link
)
add_library(lue_hdf5_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_hdf5_objects>
    PARENT_SCOPE
)
# file(GLOB
#     SOURCES
#     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
#     *.cpp *.h
# )
# install(
#     FILES
#         ${SOURCES}
#     DESTINATION _lue/lue/hdf5
#     COMPONENT python
#     # CMAKE >= 3.6 EXCLUDE_FROM_ALL
# )
