set(SOURCES
    dataset
    file
    group
    identifier
    shape
    submodule
)
add_library(luepy_hdf5_objects OBJECT
    ${SOURCES}
)
set(luepy_object_libraries
    ${luepy_object_libraries}
    $<TARGET_OBJECTS:luepy_hdf5_objects>
    PARENT_SCOPE
)
