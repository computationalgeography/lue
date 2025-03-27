# framework/*
foreach(module_name __init__.py)
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/image_land/${module_name}
        INPUT image_land/${module_name}
    )
endforeach()

# A pybind11 SHARED library is a CMake runtime output artifact. Target properties to set/get are
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME.
add_library(lue_py_image_land SHARED
    source/integrate.cpp
    source/integrate_and_allocate.cpp
    source/python_extension.cpp
    source/submodule.cpp
)

add_library(lue::py_image_land ALIAS lue_py_image_land)

target_compile_definitions(lue_py_image_land
    PRIVATE
        NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
)

set_target_properties(lue_py_image_land
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

target_include_directories(lue_py_image_land
    PRIVATE
        $<TARGET_PROPERTY:lue::py,SOURCE_DIR>/include  # bind.hpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_py_image_land
    PRIVATE
        lue::image_land
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        Python::Python
)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_image_land)
endif()

lue_install_runtime_libraries(
    TARGETS lue_py_image_land
)

if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
    # If we are using a Python interpreter from a Conda environment then we must expand RPATH
    # to include the location of the Python libraries. But if we are building a Conda package,
    # we assume Conda takes care of RPATH handling.
    set_property(
        TARGET lue_py_image_land
        APPEND
        PROPERTY INSTALL_RPATH
            ${Python_LIBRARY_DIRS}
    )
endif()

install(
    DIRECTORY
        ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/image_land
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
