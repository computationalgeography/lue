add_test_conditionally(test)

# ./*
foreach(module_name __init__.py)
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework_x/${module_name}
        INPUT ./${module_name}
    )
endforeach()

add_library(lue_py_framework_x SHARED
    src/array.cpp
    src/extension.cpp
    src/field.cpp
    # src/io.cpp
    src/local_operation.cpp
    src/scalar.cpp
    src/submodule.cpp
)

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

# set_target_properties(lue_py_framework_x
#     PROPERTIES
#         LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}
# )

target_include_directories(lue_py_framework_x
    PRIVATE
    #     ${CMAKE_CURRENT_SOURCE_DIR}/../../data_model/python/include
    #     ${CMAKE_CURRENT_BINARY_DIR}/../../python/include  # configure.hpp
    #     $<TARGET_PROPERTY:lue::py_framework,SOURCE_DIR>/src  # shape.hpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_py_framework_x
    PRIVATE
        lue::framework_api_cxx
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras

        # lue::framework_local_operation
         Python::Python

        # lue::shared
        # pybind11::module
        # pybind11::thin_lto
        # pybind11::opt_size
        # pybind11::windows_extras
        # # Boost::headers
)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_framework_x)
endif()

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
