# add_subdirectory(different_shape)
add_subdirectory(same_shape)
set(SOURCES
    # domain
    property
    property_set
    # space_box
    submodule
)
add_library(luepy_constant_size_time_omnipresent_objects OBJECT
    ${SOURCES}
)
set(luepy_object_libraries
    ${luepy_object_libraries}
    $<TARGET_OBJECTS:luepy_constant_size_time_omnipresent_objects>
    PARENT_SCOPE
)
