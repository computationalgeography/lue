add_hpx_executable(lue_airpolution
    ESSENTIAL
    SOURCES
        src/main
        src/polute_air
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_airpolution_exe SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
)

target_link_libraries(lue_airpolution_exe
    PRIVATE
        lue::data_model
        docopt
)
