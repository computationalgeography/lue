# Top level LUE Python package. This one will conditionally include all
# other submodules, based on build-type configuration parameters.

# This package is called `lue`. The submodules are called `lue.data_model`
# and `lue.framework` respectively.

add_test_conditionally(test)

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

target_link_libraries(lue_py
    PRIVATE
        lue::shared
        CONAN_PKG::pybind11
)

if(LUE_DATA_MODEL_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_data_model)
endif()

if(LUE_FRAMEWORK_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_framework)
endif()