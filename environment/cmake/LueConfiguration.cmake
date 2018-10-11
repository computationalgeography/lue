# Options to be set by the user ------------------------------------------------
option(LUE_BUILD_DATA_MODEL
    "Build LUE data model API"
    TRUE)
option(LUE_DATA_MODEL_WITH_PYTHON_API
    "Include Python API for data model"
    FALSE)
option(LUE_DATA_MODEL_WITH_UTILITIES
    "Include data model command line utilities"
    FALSE)

option(LUE_BUILD_FRAMEWORK
    "Build LUE simulation framework"
    FALSE)
option(LUE_FRAMEWORK_WITH_OPENCL
    "Include support for OpenCL"
    FALSE)
# option(LUE_FRAMEWORK_WITH_MPI
#     "Include support for MPI"
#     FALSE)
option(LUE_FRAMEWORK_WITH_DASHBOARD
    "Include dashboard for simulation framework"
    FALSE)
option(LUE_FRAMEWORK_WITH_BENCHMARKS
    "Include benchmarks"
    FALSE)

option(LUE_BUILD_DOCUMENTATION
    "Build documentation"
    FALSE)
option(LUE_BUILD_TEST
    "Build tests"
    FALSE)


# Handle internal dependencies -------------------------------------------------
if(LUE_BUILD_DATA_MODEL)
    if(LUE_DATA_MODEL_WITH_UTILITIES)
    endif()

    if(LUE_DATA_MODEL_WITH_PYTHON_API)
    endif()
endif()


if(LUE_BUILD_FRAMEWORK)
    if(LUE_FRAMEWORK_WITH_DASHBOARD)
    endif()
endif()


if(LUE_BUILD_TEST)
endif()


if(LUE_BUILD_DOCUMENTATION)
endif()


# Handle external dependencies -------------------------------------------------
if(LUE_BUILD_DATA_MODEL)
    set(DEVBASE_HDF5_REQUIRED TRUE)
    list(APPEND DEVBASE_REQUIRED_HDF5_COMPONENTS
        C)  # HL
    set(DEVBASE_FMT_REQUIRED TRUE)

    set(DEVBASE_BOOST_REQUIRED TRUE)
    list(APPEND DEVBASE_REQUIRED_BOOST_COMPONENTS
        filesystem system)

    # if(LUE_API_WITH_MPI)
        # set(DEVBASE_MPI_REQUIRED TRUE)
        set(HDF5_PREFER_PARALLEL TRUE)
    # endif()


    if(LUE_DATA_MODEL_WITH_UTILITIES)
        set(DEVBASE_DOCOPT_REQUIRED TRUE)
        set(LUE_GDAL_REQUIRED TRUE)
        set(DEVBASE_NLOHMANN_JSON_REQUIRED TRUE)
    endif()

    if(LUE_DATA_MODEL_WITH_PYTHON_API)
        set(DEVBASE_GUIDELINE_SUPPORT_LIBRARY_REQUIRED TRUE)
        set(DEVBASE_PYBIND11_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_FRAMEWORK)
    # TODO Add all dependencies of framework
    # - boost, fmt, ...
    set(DEVBASE_FMT_REQUIRED TRUE)
    set(DEVBASE_HPX_REQUIRED TRUE)

    if(LUE_FRAMEWORK_WITH_OPENCL)
        set(DEVBASE_OPENCL_REQUIRED TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_DASHBOARD)
        set(DEVBASE_OPENGL_REQUIRED TRUE)
        set(DEVBASE_SDL2_REQUIRED TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    endif()
endif()


if(LUE_BUILD_TEST)
    set(DEVBASE_BUILD_TEST TRUE)
    set(DEVBASE_BOOST_REQUIRED TRUE)
    list(APPEND DEVBASE_REQUIRED_BOOST_COMPONENTS
        filesystem system unit_test_framework)
endif()


if(LUE_BUILD_DOCUMENTATION)
    set(DEVBASE_DOXYGEN_REQUIRED TRUE)
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


# Find external packages -------------------------------------------------------
if(DEVBASE_BOOST_REQUIRED)
    find_package(Boost REQUIRED
        COMPONENTS ${DEVBASE_REQUIRED_BOOST_COMPONENTS})
    unset(DEVBASE_BOOST_REQUIRED)
endif()


if(DEVBASE_DOXYGEN_REQUIRED)
    find_package(Doxygen REQUIRED dot)
    unset(DEVBASE_DOXYGEN_REQUIRED)
endif()


if(LUE_GDAL_REQUIRED)
    find_package(GDAL 2 REQUIRED)
    unset(LUE_GDAL_REQUIRED)
endif()


if(DEVBASE_HPX_REQUIRED)
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

    unset(DEVBASE_HPX_REQUIRED)
endif()


if(DEVBASE_OPENCL_REQUIRED)
    find_package(OpenCL REQUIRED)
    unset(DEVBASE_OPENCL_REQUIRED)
endif()


if(DEVBASE_OPENGL_REQUIRED)
    find_package(OpenGL REQUIRED)
    unset(DEVBASE_OPENGL_REQUIRED)
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


if(DEVBASE_SDL2_REQUIRED)
    find_package(SDL2 REQUIRED)
    unset(DEVBASE_SDL2_REQUIRED)
endif()
