set(SOURCES
    value
    property
)
add_library(lue_constant_size_time_omnipresent_different_shape_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_constant_size_time_omnipresent_different_shape_objects>
    PARENT_SCOPE
)


file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
)
file(RELATIVE_PATH destination
    ${LUE_SOURCE_PREFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
install(
    FILES
        ${SOURCES}
    DESTINATION ${destination}
    COMPONENT python
)
