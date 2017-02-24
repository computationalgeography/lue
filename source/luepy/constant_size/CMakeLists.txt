add_subdirectory(time)
set(SOURCES
    property
    property_set
    submodule
)
add_library(luepy_constant_size_objects OBJECT
    ${SOURCES}
)
set(luepy_object_libraries
    ${luepy_object_libraries}
    $<TARGET_OBJECTS:luepy_constant_size_objects>
    PARENT_SCOPE
)
