add_test_conditionally(test)
add_subdirectory(hdf5)

set(SOURCES
    array
    collection
    dataset
    domain
    domain_configuration
    exception
    phenomena
    phenomenon
    properties
    property
    property_set
    property_sets
    space_domain
    space_domain_configuration
    time_domain
    time_domain_configuration
    universe
    universes
)
add_library(lue_cxx_api_objects OBJECT
    ${SOURCES}
)
add_library(lue_cxx STATIC
    $<TARGET_OBJECTS:lue_cxx_api_hdf5_objects>
    $<TARGET_OBJECTS:lue_cxx_api_objects>
)
target_link_libraries(lue_cxx
    lue_c
    # lue_cxx depends on lue_c which depends on hdf5
    # lue_cxx should not depend on hdf5
    # ${HDF5_LIBRARIES}  Don't uncomment!
)
install(
    TARGETS lue_cxx
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
### # get_target_property(sources lue_cxx SOURCES)
### # message(${sources})
### install(
###     FILES
###         xxx
###     DESTINATION _lue/lue/cxx_api
###     COMPONENT python
###     # CMAKE >= 3.6 EXCLUDE_FROM_ALL
### )
