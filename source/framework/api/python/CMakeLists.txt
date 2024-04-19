# set(LUE_NANOBIND_REQUIRED TRUE)
#
# if(LUE_NANOBIND_REQUIRED)
#     FetchContent_Declare(nanobind
#         GIT_REPOSITORY https://github.com/wjakob/nanobind.git
#         GIT_TAG 80a30c8efb093b14f0e744bc7f6a9ef34beb3f7f  # v1.9.2
#         SYSTEM
#         FIND_PACKAGE_ARGS
#     )
#     FetchContent_MakeAvailable(nanobind)
# endif()


add_test_conditionally(test)

add_library(lue_py_x MODULE
    src/extension.cpp
)

add_library(lue::py_x ALIAS lue_py_x)

set_target_properties(lue_py_x
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}
)

target_link_libraries(lue_py_x
    PRIVATE
        lue::shared
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        # Boost::headers
)

pybind11_extension(lue_py_x)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_x)
endif()
