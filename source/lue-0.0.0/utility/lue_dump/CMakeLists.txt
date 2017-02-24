set(SOURCES
    main
    dump
    # txt_dumper
    txt_format
)
add_executable(lue_dump ${SOURCES})

target_link_libraries(lue_dump
    lue_utility
    lue_cxx
    ${DOCOPT_LIBRARIES}
    # ${Boost_FILESYSTEM_LIBRARY}
    # ${Boost_SYSTEM_LIBRARY}
)
install(
    TARGETS lue_dump
    RUNTIME DESTINATION bin
)
