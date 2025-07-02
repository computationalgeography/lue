# ./*
foreach(module_name __init__.py)
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework_x/${module_name}
        INPUT ./${module_name}
    )
endforeach()

pybind11_add_module(lue_py_framework_x SHARED THIN_LTO OPT_SIZE
    source/array.cpp
    source/extension.cpp
    source/field.cpp
    # source/io.cpp
    source/local_operation.cpp
    source/scalar.cpp
    source/submodule.cpp
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
    #     $<TARGET_PROPERTY:lue::py_framework,SOURCE_DIR>/source  # shape.hpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
)

target_link_libraries(lue_py_framework_x
    PRIVATE
        lue::framework_api_cxx
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
