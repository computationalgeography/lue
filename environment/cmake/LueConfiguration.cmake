# Short-cuts
option(LUE_BUILD_ALL
    "Build everything, except for documentation and tests"
    FALSE)
option(LUE_WITH_ALL
    "Support all features"  # , except for MPI"
    FALSE)

# High level targets
option(LUE_BUILD_PYTHON_API
    "Build Python-API"
    FALSE)
option(LUE_BUILD_UTILITIES
    "Build LUE command line utilites"
    TRUE)
option(LUE_BUILD_ALGORITHM
    "Build LUE algorithm library"
    FALSE)

# Lower level targets
option(LUE_BUILD_HL_API
    "Build HL-API (implies LUE_BUILD_CXX_API)"
    TRUE)
option(LUE_BUILD_CXX_API
    "Build CXX-API (implies LUE_BUILD_HDF5_API)"
    TRUE)
option(LUE_BUILD_HDF5_API
    "Build C++ wrapper library around HDF5 C-API"
    TRUE)

# Targets relevant for developers
option(LUE_BUILD_DOCUMENTATION
    "Build documentation"
    FALSE)
option(LUE_BUILD_TEST
    "Build tests"
    FALSE)

# Optional features
# option(LUE_API_WITH_MPI
#     "Include support for MPI"
#     FALSE)


# Handle internal dependencies
if(LUE_BUILD_ALL)
    set(LUE_BUILD_PYTHON_API TRUE)
    set(LUE_BUILD_UTILITIES TRUE)
endif()

if(LUE_BUILD_PYTHON_API)
    set(LUE_BUILD_HL_API TRUE)
endif()

if(LUE_BUILD_UTILITIES)
    set(LUE_BUILD_HL_API TRUE)
endif()

if(LUE_BUILD_HL_API)
    set(LUE_BUILD_CXX_API TRUE)
endif()

if(LUE_BUILD_CXX_API)
    set(LUE_BUILD_HDF5_API TRUE)
endif()

if(LUE_BUILD_TEST)
    set(DEVBASE_BUILD_TEST TRUE)
    set(LUE_BUILD_LUE_TEST TRUE)
endif()


if(LUE_WITH_ALL)
    # Turn on features when necessary.
    # ...
endif()


# Handle external dependencies.
if(LUE_BUILD_HDF5_API)
    set(DEVBASE_HDF5_REQUIRED TRUE)
    list(APPEND DEVBASE_REQUIRED_HDF5_COMPONENTS
        C)  # HL
    set(DEVBASE_FMT_REQUIRED TRUE)
endif()

if(LUE_BUILD_CXX_API)
    set(DEVBASE_BOOST_REQUIRED TRUE)
    list(APPEND DEVBASE_REQUIRED_BOOST_COMPONENTS
        filesystem system)

    # if(LUE_API_WITH_MPI)
        # set(DEVBASE_MPI_REQUIRED TRUE)
        set(HDF5_PREFER_PARALLEL TRUE)
    # endif()
endif()


if(LUE_BUILD_UTILITIES)
    set(DEVBASE_DOCOPT_REQUIRED TRUE)
    set(LUE_GDAL_REQUIRED TRUE)
    set(DEVBASE_NLOHMANN_JSON_REQUIRED TRUE)

    set(LUE_BUILD_LUE_DUMP TRUE)
    set(LUE_BUILD_LUE_TRANSLATE TRUE)
    set(LUE_BUILD_LUE_UTILITY TRUE)
    set(LUE_BUILD_LUE_VALIDATE TRUE)
endif()


if(LUE_BUILD_PYTHON_API)
    set(DEVBASE_GUIDELINE_SUPPORT_LIBRARY_REQUIRED TRUE)
    set(DEVBASE_PYBIND11_REQUIRED TRUE)
endif()


if(LUE_BUILD_TEST)
    set(DEVBASE_BOOST_REQUIRED TRUE)
    list(APPEND DEVBASE_REQUIRED_BOOST_COMPONENTS
        filesystem system unit_test_framework)
endif()


if(LUE_BUILD_ALGORITHM)
    # Build HPX with support for
    # - boost (cxxflags=-std=c++11) (required)
    # - hwloc (required, most recent version)
    # - Google perftools (+ libunwind) (recommended)
    # - OpenMPI (cluster) (recommended)
    # - HDF5 (build with threadsafety, CFLAGS/CPPFLAGS='-DHDatexit=""')
    # - PAPI (recommended)

    # HPX_WITH_CXX0Y
    # HPX_WITH_UNWRAPPED_COMPATIBILITY:BOOL=OFF
    # HPX_WITH_VIM_YCM:BOOL=ON
    # HPX_WITH_INCLUSIVE_SCAN_COMPATIBILITY:BOOL=OFF
    # HPX_WITH_LOCAL_DATAFLOW_COMPATIBILITY:BOOL=OFF

    # HPX_WITH_BUILD_EXAMPLES=ON
    # HPX_WITH_BUILD_EXAMPLES_HDF5=ON
    # HPX_WITH_EXAMPLES_OPENMP:BOOL=ON

    # HPX_WITH_TESTS=ON
    # HPX_WITH_TOOLS=ON

    # HPX_WITH_PARCELPORT_ACTION_COUNTERS:BOOL=ON
    # HPX_WITH_PARCELPORT_MPI:BOOL=ON

    # HPX_WITH_GOOGLE_PERFTOOLS:BOOL=ON
    # HPX_WITH_PAPI:BOOL=ON

    # Check HPX_WITH_MALLOC

    # TODO Build HPX yourself, using a seperate script. We probably need
    #     various versions. Or use toolchain files for machines?

    find_package(HPX REQUIRED)

    if(HPX_FOUND)
        message(STATUS "Found HPX")
        message(STATUS "  includes : ${HPX_INCLUDE_DIRS}")
        message(STATUS "  libraries: ${HPX_LIBRARIES}")

        # Check whether we are using the same build type as HPX
        if (NOT "${HPX_BUILD_TYPE}" STREQUAL "${CMAKE_BUILD_TYPE}")
            message(WARNING
                "CMAKE_BUILD_TYPE does not match HPX_BUILD_TYPE: "
                "\"${CMAKE_BUILD_TYPE}\" != \"${HPX_BUILD_TYPE}\"\n"
                "ABI compatibility is not guaranteed. Expect link errors.")
        endif()
    endif()
endif()


if(LUE_BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED dot)
    set(DEVBASE_GRAPHVIZ_REQUIRED TRUE)
    set(DEVBASE_SPHINX_REQUIRED TRUE)

    find_program(EDIT_DOT_GRAPH
        edit_dot_graph.py
        PATHS ${PROJECT_SOURCE_DIR}/devbase/script
        NO_DEFAULT_PATH
    )
    if(NOT EDIT_DOT_GRAPH)
        message(FATAL_ERROR "edit_dot_graph.py not found")
    endif()
endif()


if(DEVBASE_BOOST_REQUIRED)
    find_package(Boost REQUIRED
        COMPONENTS ${DEVBASE_REQUIRED_BOOST_COMPONENTS})
    unset(DEVBASE_BOOST_REQUIRED)
endif()


if(DEVBASE_PYBIND11_REQUIRED)
    # Find Pybind11. It will look for Python.
    find_package(pybind11 REQUIRED)
    unset(DEVBASE_PYBIND11_REQUIRED)

    # Given Python found, figure out where the NumPy headers are. We don't
    # want to pick up headers from another prefix than the prefix of the
    # Python interpreter.
    execute_process(COMMAND "${PYTHON_EXECUTABLE}" -c
        "import numpy as np; print(\"{};{}\".format(np.__version__, np.get_include()));"
        RESULT_VARIABLE numpy_search_result
        OUTPUT_VARIABLE numpy_search_output
        ERROR_VARIABLE numpy_search_error
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT numpy_search_result MATCHES 0)
        message(FATAL_ERROR
            "${PYTHON_EXECUTABLE} is unable to import numpy:\n${numpy_search_error}")
    else()
        list(GET numpy_search_output -2 numpy_version)
        list(GET numpy_search_output -1 NUMPY_INCLUDE_DIRS)

        message(STATUS
            "Found NumPy ${numpy_version} headers in ${NUMPY_INCLUDE_DIRS}")
    endif()
endif()


if(LUE_GDAL_REQUIRED)
    find_package(GDAL 2 REQUIRED)
    unset(LUE_GDAL_REQUIRED)
endif()
