# ./*
foreach(module_name __init__.py)
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework_x/${module_name}
        INPUT ./${module_name}
    )
endforeach()

add_library(lue_py_framework_x SHARED)
add_library(lue::py_framework_x ALIAS lue_py_framework_x)

target_compile_definitions(lue_py_framework_x
    PRIVATE
        NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
)

set_target_properties(lue_py_framework_x
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

target_sources(lue_py_framework_x
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
    PRIVATE
        source/array.cpp
        source/extension.cpp
        source/field.cpp
        source/focal_operation.cpp
        # source/io.cpp
        source/kernel.cpp
        source/local_operation.cpp
        source/routing_operation.cpp
        source/scalar.cpp
        source/submodule.cpp
)

target_link_libraries(lue_py_framework_x
    PRIVATE
        lue::framework_api_cxx
        pybind11::module
)

lue_install_runtime_libraries(
    TARGETS lue_py_framework_x
)

if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
    # If we are using a Python interpreter from a Conda environment then we must expand RPATH
    # to include the location of the Python libraries. But if we are building a Conda package,
    # we assume Conda takes care of RPATH handling.
    set_property(
        TARGET lue_py_framework_x
        APPEND
        PROPERTY INSTALL_RPATH
            ${Python_LIBRARY_DIRS}
    )
endif()

# TODO
# install(
#     DIRECTORY
#         ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework
#     DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
#     COMPONENT lue_runtime
# )

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
