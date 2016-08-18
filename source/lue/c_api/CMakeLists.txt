add_test_conditionally(test)
add_subdirectory(hdf5)

set(SOURCES
    collection
    dataset
    domain
    phenomenon
    phenomena
    properties
    property
    property_set
    property_sets
    space_domain
    time_domain
    universe
    universes
)
add_library(lue_c_api_objects OBJECT
    ${SOURCES}
)
add_library(lue_c STATIC
    $<TARGET_OBJECTS:lue_c_api_hdf5_objects>
    $<TARGET_OBJECTS:lue_c_api_objects>
)
target_link_libraries(lue_c
    ${HDF5_C_LIBRARIES}
    ${HDF5_LIBRARIES}
)
install(
    TARGETS lue_c
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h *.c
)
install(
    FILES
        ${SOURCES}
    DESTINATION _lue/lue/c_api
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
