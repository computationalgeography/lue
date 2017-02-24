set(SOURCES
    value
    property
)
add_library(lue_constant_size_time_omnipresent_same_shape_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_constant_size_time_omnipresent_same_shape_objects>
    PARENT_SCOPE
)
