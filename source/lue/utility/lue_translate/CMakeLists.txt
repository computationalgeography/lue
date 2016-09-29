add_subdirectory(time)

set(LIB_SOURCES
    file_stack
    gdal
    gdal_block
    print_message
    progress_indicator
    stopwatch
)
add_library(lue_translate_lib ${LIB_SOURCES})
target_link_libraries(lue_translate_lib
    lue_translate_time
)

set(SOURCES
    main
)
add_executable(lue_translate ${SOURCES})

target_link_libraries(lue_translate
    lue_translate_lib
    lue_cxx
    ${DOCOPT_LIBRARIES}
    ${GDAL_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)
install(
    TARGETS lue_translate
    RUNTIME DESTINATION bin
)
