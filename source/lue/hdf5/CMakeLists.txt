add_test_conditionally(test)

# TODO Determine how to test whether hsize_t equals some standard native
#      type. Maybe using some C++ snippet with is_same static assert.

configure_file(
    configure.h.in
    configure.h
)
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
