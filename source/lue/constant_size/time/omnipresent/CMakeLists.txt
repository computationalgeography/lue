add_test_conditionally(test)
add_subdirectory(different_shape)
add_subdirectory(same_shape)
set(SOURCES
    domain
    property
    property_set
)
add_library(lue_constant_size_time_omnipresent_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_constant_size_time_omnipresent_objects>
    PARENT_SCOPE
)
