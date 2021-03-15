# Top level LUE Python package. This one will conditionally include all
# other submodules, based on build-type configuration parameters.

# This package is called `lue`. The submodules are called `lue.data_model`
# and `lue.framework` respectively.

add_test_conditionally(test)

configure_file(
    include/lue/py/configure.hpp.in
    include/lue/py/configure.hpp
)

pybind11_add_module(lue_py MODULE SYSTEM THIN_LTO
    src/python_extension
)

add_library(lue::py ALIAS lue_py)

# target_compile_definitions(lue_py
#     PRIVATE
#         NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
# )

set_target_properties(lue_py
    PROPERTIES
        OUTPUT_NAME lue
)

# target_include_directories(lue_py
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
#         $<INSTALL_INTERFACE:include>
# )

target_link_libraries(lue_py
    PRIVATE
        lue::shared
        pybind11::pybind11
        Boost::headers
)

if(LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_data_model)
endif()

if(LUE_BUILD_FRAMEWORK AND LUE_FRAMEWORK_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_framework)
endif()

install(
    TARGETS
        lue_py
    LIBRARY DESTINATION ${LUE_PYTHON_API_INSTALL_DIR}
    RUNTIME DESTINATION ${LUE_PYTHON_API_INSTALL_DIR}
)
