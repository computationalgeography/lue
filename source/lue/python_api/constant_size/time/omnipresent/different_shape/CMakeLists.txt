set(SOURCES
    item
    property
    submodule
)
add_library(different_shape_objects OBJECT
    ${SOURCES}
)
set(lue_python_api_object_libraries
    ${lue_python_api_object_libraries}
    $<TARGET_OBJECTS:different_shape_objects>
    PARENT_SCOPE
)
