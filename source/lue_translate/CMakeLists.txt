add_subdirectory(command)
add_subdirectory(format)

set(LIB_SOURCES
)
add_library(lue_translate_lib STATIC
    $<TARGET_OBJECTS:lue_translate_command_objects>
    $<TARGET_OBJECTS:lue_translate_format_objects>
    ${LIB_SOURCES}
)

set(SOURCES
    main
    translate
)
add_executable(lue_translate
    ${SOURCES}
)

target_link_libraries(lue_translate
    lue_translate_lib
    lue_utility
    luehl
    lue
    ${GDAL_LIBRARIES}
)

install(
    TARGETS lue_translate
    RUNTIME DESTINATION bin
)
