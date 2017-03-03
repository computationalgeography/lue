add_subdirectory(time)
set(SOURCES
    domain
    property
    property_set
    space_domain
)
add_library(lue_constant_size_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_constant_size_objects>
    PARENT_SCOPE
)
