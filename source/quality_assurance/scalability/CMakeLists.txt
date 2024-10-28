add_subdirectory(command)
add_subdirectory(script)

add_test_conditionally(test)

install(
    DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/command
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)
