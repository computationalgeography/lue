# FIXME
# add_test_conditionally(test)

pybind11_add_module(lue_py_framework SHARED SYSTEM THIN_LTO
    src/algorithm/local_operation
    src/core/partitioned_array
    src/python_extension
    src/submodule
)

add_library(lue::py_framework ALIAS lue_py_framework)

target_compile_definitions(lue_py_framework
    PRIVATE
        NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
)

set_target_properties(lue_py_framework
    PROPERTIES
        OUTPUT_NAME lue_py_framework
)

### # Explicitly add the location of the numpy headers to the list. Otherwise
### # the wrong header might be picked up, resulting in runtime errors.
### target_include_directories(lue_py_framework SYSTEM
###     PRIVATE
###         ${NUMPY_INCLUDE_DIRS}
### )

target_include_directories(lue_py_framework
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../data_model/python/include
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_py_framework
    PRIVATE
        # # $<$<TARGET_EXISTS:lue::data_model_test>:lue::data_model_test>
        lue::framework_algorithm
        #     # lue::data_model
        #     # CONAN_PKG::gsl_microsoft
        # Boost::program_options
        CONAN_PKG::pybind11
)
