add_executable(lue.framework.algorithm.example.ndvi ndvi.cpp)

target_link_libraries(lue.framework.algorithm.example.ndvi
    PRIVATE
        lue::framework_local_operation
        lue::framework_miscellaneous_operation
)

lue_install_executables(
    TARGETS
        lue.framework.algorithm.example.ndvi
)
