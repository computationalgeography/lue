set(LIB_SOURCES
    dot
)
add_library(lue_translate_lib ${LIB_SOURCES})

set(SOURCES
    main
    translate
)
add_executable(lue_translate ${SOURCES})

target_link_libraries(lue_translate
    lue_utility
    lue_translate_lib
    lue
    ${DOCOPT_LIBRARIES}
)

install(
    TARGETS lue_translate
    RUNTIME DESTINATION bin
)
