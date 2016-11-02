set(SOURCES
    error
    main
    validate
)
add_executable(lue_validate ${SOURCES})

target_link_libraries(lue_validate
    lue_utility
    lue_cxx
    ${DOCOPT_LIBRARIES}
    ${GDAL_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)
install(
    TARGETS lue_validate
    RUNTIME DESTINATION bin
)
