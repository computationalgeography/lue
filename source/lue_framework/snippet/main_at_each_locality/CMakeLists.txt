add_hpx_executable(lue_main_at_each_locality
    ESSENTIAL
    SOURCES
        main
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_main_at_each_locality_exe SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
)
target_link_libraries(lue_main_at_each_locality_exe
    PRIVATE
        fmt::fmt
)
