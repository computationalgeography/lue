add_hpx_executable(1d_stencil_4
    ESSENTIAL
    SOURCES
        1d_stencil_4
    # COMPONENT_DEPENDENCIES
    #     1d_stencil_4
)

target_include_directories(1d_stencil_4_exe SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
)
