add_subdirectory(omnipresent)
set(SOURCES
    submodule
)
add_library(luepy_constant_size_time_objects OBJECT
    ${SOURCES}
)
set(luepy_object_libraries
    ${luepy_object_libraries}
    $<TARGET_OBJECTS:luepy_constant_size_time_objects>
    PARENT_SCOPE
)
