add_library(lue_py_qa SHARED)
add_library(lue::py_qa ALIAS lue_py_qa)

set_target_properties(lue_py_qa
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

target_sources(lue_py_qa
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE

    source/python_extension.cpp
    source/submodule.cpp
)

target_link_libraries(lue_py_qa
    PRIVATE
        lue::qa
        lue::shared
)

lue_configure_python_shared_library(lue_py_qa)

lue_install_runtime_libraries(
    TARGETS lue_py_qa
)

install(
    DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/qa
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)

add_subdirectory(qa)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
