set(SOURCES
    item
    property
    submodule
)
add_library(same_shape_objects OBJECT
    ${SOURCES}
)
set(lue_python_api_object_libraries
    ${lue_python_api_object_libraries}
    $<TARGET_OBJECTS:same_shape_objects>
    PARENT_SCOPE
)
