include(CheckCXXSymbolExists)
include(CMakeDependentOption)


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

option(LUE_BUILD_GDAL
    "Build LUE GDAL wrapper library"
    TRUE)

option(LUE_BUILD_FRAMEWORK
    "Build LUE simulation framework"
    TRUE)
option(LUE_FRAMEWORK_WITH_PYTHON_API
    "Include Python API for modelling framework"
    FALSE)
option(LUE_FRAMEWORK_WITH_PCRASTER_PYTHON_API
    "Include PCRaster Python sub-package"
    TRUE)
option(LUE_FRAMEWORK_WITH_IMAGE_LAND
    "Include operations required for the IMAGE land-use allocation model"
    FALSE)

option(LUE_BUILD_VIEW
    "Build LUE data model viewer"
    TRUE)

option(LUE_BUILD_DOCUMENTATION
    "Build documentation"
    FALSE)

option(LUE_BUILD_QUALITY_ASSURANCE
    "Include support for quality assurance"
    FALSE)
option(LUE_QUALITY_ASSURANCE_WITH_PYTHON_API
    "Include Python API for quality assurance"
    FALSE)
option(LUE_QUALITY_ASSURANCE_WITH_TESTS
    "Include tests"
    FALSE)
set(LUE_QUALITY_ASSURANCE_TEST_NR_LOCALITIES_PER_TEST
    1 CACHE STRING "Number of localities to use")
set(LUE_QUALITY_ASSURANCE_TEST_NR_THREADS_PER_LOCALITY
    1 CACHE STRING "Number of threads to use")
set(LUE_QUALITY_ASSURANCE_TEST_HPX_RUNWRAPPER
    none CACHE STRING "Which runwrapper to use (see hpxrun.py)")
set(LUE_QUALITY_ASSURANCE_TEST_HPX_PARCELPORT
    tcp CACHE STRING "Which parcelport to use (see hpxrun.py)")

# Options tweaking the behaviour of the software
option(LUE_ASSERT_CONDITIONS
    "Assert conditions placed in the source code"
    FALSE)
option(LUE_VALIDATE_IDXS
    "Validate array indices are within array bounds (expensive!)"
    FALSE)

# Options related to the availability of external packages.
if(WIN32)
    set(LUE_HAVE_DOXYGEN_INIT FALSE)
elseif(APPLE)
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
else()
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
endif()

function(lue_have_option name)
    option(LUE_HAVE_${name}
        "If ${name} is required, assume it is installed"
        ${LUE_HAVE_${name}_INIT})
endfunction()

lue_have_option(DOXYGEN)

# Update / remove once we've got Vulkan sorted out.
# Whether or not to use Vulkan, instead of OpenGL.
set(LUE_VIEW_USE_VULKAN TRUE)


# Handle internal dependencies -------------------------------------------------
if(LUE_BUILD_FRAMEWORK)
    set(LUE_BUILD_DATA_MODEL TRUE)
    set(LUE_BUILD_GDAL TRUE)
endif()


if(LUE_BUILD_VIEW)
    set(LUE_BUILD_DATA_MODEL TRUE)
    set(LUE_BUILD_IMGUI TRUE)
endif()


if(LUE_BUILD_DATA_MODEL)
    if(LUE_DATA_MODEL_WITH_UTILITIES)
        set(LUE_BUILD_GDAL TRUE)
    endif()
endif()


if(LUE_BUILD_FRAMEWORK)
    set(LUE_TEMPLATIZE "${PROJECT_SOURCE_DIR}/environment/script/templatize.py")

    # Types for which templates are instantiated
    set(LUE_FRAMEWORK_BOOLEAN_ELEMENT
        "std::uint8_t" CACHE STRING "Type to use for representing Boolean values")
    set_property(CACHE LUE_FRAMEWORK_BOOLEAN_ELEMENT
        PROPERTY
            STRINGS
                "std::uint8_t"
                "std::int8_t"
    )

    set(LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT
        "std::uint8_t" CACHE STRING "Type to use for representing flow directions")
    set_property(CACHE LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT
        PROPERTY
            STRINGS
                "std::uint8_t"
                "std::int8_t"
    )

    set(LUE_FRAMEWORK_COUNT_ELEMENT
        "std::uint64_t" CACHE STRING "Type to use for representing counts")
    set_property(CACHE LUE_FRAMEWORK_COUNT_ELEMENT
        PROPERTY
            STRINGS
                "std::uint64_t"
                "std::int64_t"
                "std::uint32_t"
                "std::int32_t"
    )

    set(LUE_FRAMEWORK_ID_ELEMENT
        "std::uint64_t" CACHE STRING "Type to use for representing IDs")
    set_property(CACHE LUE_FRAMEWORK_ID_ELEMENT
        PROPERTY
            STRINGS
                "std::uint64_t"
                "std::int64_t"
                "std::uint32_t"
                "std::int32_t"
    )

    set(LUE_FRAMEWORK_INDEX_ELEMENT
        "std::uint64_t" CACHE STRING "Type to use for representing indices")
    set_property(CACHE LUE_FRAMEWORK_INDEX_ELEMENT
        PROPERTY
            STRINGS
                "std::uint64_t"
                "std::int64_t"
                "std::uint32_t"
                "std::int32_t"
    )

    set(LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS
        std::int8_t std::int32_t std::int64_t CACHE STRING "Type(s) to use for representing signed integral values")
    set_property(CACHE LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS
        PROPERTY
            STRINGS
                "std::int8_t\;std::int32_t\;std::int64_t"
                "std::int8_t\;std::int32_t"
                "std::int8_t"
                "std::int32_t\;std::int64_t"
                "std::int32_t"
    )

    set(LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS
        std::uint8_t std::uint32_t std::uint64_t CACHE STRING "Type(s) to use for representing signed integral values")
    set_property(CACHE LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS
        PROPERTY
            STRINGS
                "std::uint8_t\;std::uint32_t\;std::uint64_t"
                "std::uint8_t\;std::uint32_t"
                "std::uint8_t"
                "std::uint32_t\;std::uint64_t"
                "std::uint32_t"
    )

    set(LUE_FRAMEWORK_INTEGRAL_ELEMENTS
        ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}
        ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})

    set(LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS
        float double CACHE STRING
        "Type(s) to use for representing floating point values"
    )
    set_property(CACHE LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS
        PROPERTY
            STRINGS
                "float\;double"
                "float"
                "double"
    )

    set(LUE_FRAMEWORK_ELEMENTS
        ${LUE_FRAMEWORK_INTEGRAL_ELEMENTS}
        ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})

    set(LUE_FRAMEWORK_MATERIAL_ELEMENTS
        ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}
        ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS}
        ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})

    set(LUE_FRAMEWORK_ZONE_ELEMENTS
        ${LUE_FRAMEWORK_INTEGRAL_ELEMENTS})

    # Ranks for which templates are instantiated. Rank 2 has to be there for the spatial algorithms.
    set(LUE_FRAMEWORK_RANKS
        2 CACHE STRING
        "Rank(s) of arrays")
    set_property(CACHE LUE_FRAMEWORK_RANKS
        PROPERTY
            STRINGS
                "2"
    )

    include(CheckTypeSize)
    set(CMAKE_EXTRA_INCLUDE_FILES "chrono")
    check_type_size(
        "std::chrono::high_resolution_clock::duration::rep"
        LUE_FRAMEWORK_CLOCK_TICK_SIZE
        LANGUAGE "CXX"
    )
    unset(CMAKE_EXTRA_INCLUDE_FILES)

    if(${LUE_FRAMEWORK_CLOCK_TICK_SIZE} EQUAL "8")
        set(LUE_FRAMEWORK_CLOCK_TICK_ELEMENT "std::uint64_t")
    else()
        message(FATAL_ERROR "Clock tick size is ${LUE_FRAMEWORK_CLOCK_TICK_SIZE}, which we haven't seen yet. Update CMake logic.")
    endif()

    # Policies for which templates are instantiated
    set(LUE_FRAMEWORK_ALGORITHM_POLICIES
        "DefaultValuePolicies" CACHE STRING
        "Algorithm policies"
    )
    set_property(CACHE LUE_FRAMEWORK_ALGORITHM_POLICIES
        PROPERTY
            STRINGS
                "DefaultValuePolicies"
                "DefaultValuePolicies\;DefaultPolicies"
                "DefaultPolicies"
    )

    LIST(FIND LUE_FRAMEWORK_ALGORITHM_POLICIES "DefaultPolicies" TYPE_PRESENT)
    if(TYPE_PRESENT GREATER_EQUAL 0)
        set(LUE_FRAMEWORK_ALGORITHM_DEFAULT_POLICIES_ENABLED TRUE)
    else()
        set(LUE_FRAMEWORK_ALGORITHM_DEFAULT_POLICIES_ENABLED FALSE)
    endif()

    LIST(FIND LUE_FRAMEWORK_ALGORITHM_POLICIES "DefaultValuePolicies" TYPE_PRESENT)
    if(TYPE_PRESENT GREATER_EQUAL 0)
        set(LUE_FRAMEWORK_ALGORITHM_DEFAULT_VALUE_POLICIES_ENABLED TRUE)
    else()
        set(LUE_FRAMEWORK_ALGORITHM_DEFAULT_VALUE_POLICIES_ENABLED FALSE)
    endif()

    # Validate settings
    LIST(FIND LUE_FRAMEWORK_INTEGRAL_ELEMENTS ${LUE_FRAMEWORK_BOOLEAN_ELEMENT} TYPE_PRESENT)
    if(TYPE_PRESENT EQUAL -1)
        message(FATAL_ERROR "The type used for LUE_FRAMEWORK_BOOLEAN_ELEMENT (${LUE_FRAMEWORK_BOOLEAN_ELEMENT}) must be part of either LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}) or LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})")
    endif()

    LIST(FIND LUE_FRAMEWORK_INTEGRAL_ELEMENTS ${LUE_FRAMEWORK_COUNT_ELEMENT} TYPE_PRESENT)
    if(TYPE_PRESENT EQUAL -1)
        message(FATAL_ERROR "The type used for LUE_FRAMEWORK_COUNT_ELEMENT (${LUE_FRAMEWORK_COUNT_ELEMENT}) must be part of either LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}) or LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})")
    endif()

    LIST(FIND LUE_FRAMEWORK_INTEGRAL_ELEMENTS ${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT} TYPE_PRESENT)
    if(TYPE_PRESENT EQUAL -1)
        message(FATAL_ERROR "The type used for LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT (${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}) must be part of either LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}) or LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})")
    endif()

    LIST(FIND LUE_FRAMEWORK_INTEGRAL_ELEMENTS ${LUE_FRAMEWORK_ID_ELEMENT} TYPE_PRESENT)
    if(TYPE_PRESENT EQUAL -1)
        message(FATAL_ERROR "The type used for LUE_FRAMEWORK_ID_ELEMENT (${LUE_FRAMEWORK_ID_ELEMENT}) must be part of either LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}) or LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})")
    endif()

    LIST(FIND LUE_FRAMEWORK_INTEGRAL_ELEMENTS ${LUE_FRAMEWORK_INDEX_ELEMENT} TYPE_PRESENT)
    if(TYPE_PRESENT EQUAL -1)
        message(FATAL_ERROR "The type used for LUE_FRAMEWORK_INDEX_ELEMENT (${LUE_FRAMEWORK_INDEX_ELEMENT}) must be part of either LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}) or LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS (currently: ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})")
    endif()

    set(LUE_FRAMEWORK_LOCALITY_ID_ELEMENT_AVAILABLE FALSE)
    LIST(FIND LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS "std::uint32_t" TYPE_PRESENT)
    if(TYPE_PRESENT GREATER_EQUAL 0)
        set(LUE_FRAMEWORK_LOCALITY_ID_ELEMENT_AVAILABLE TRUE)
    endif()

    set(LUE_FRAMEWORK_CLOCK_TICK_ELEMENT_AVAILABLE FALSE)
    LIST(FIND LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS ${LUE_FRAMEWORK_CLOCK_TICK_ELEMENT} TYPE_PRESENT)
    if(TYPE_PRESENT GREATER_EQUAL 0)
        set(LUE_FRAMEWORK_CLOCK_TICK_ELEMENT_AVAILABLE TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_PYTHON_API AND LUE_FRAMEWORK_WITH_PCRASTER_PYTHON_API)
        # For the PCRaster Python sub-package, three types are required:
        # - boolean / flow_direction: std::uint8
        # - nominal, ordinal: std::int32
        # - scalar, directional: float
        # Verify these types are available. If not, error out with a message that allows the user to skip
        # configuraing the build for this sub-package.

        unset(error_messages)

        if(NOT LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT STREQUAL "std::uint8_t")
            list(APPEND error_messages
                "The PCRaster Python sub-package requires LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT to be std::uint8_t (currently: ${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT})")
        endif()

        if(NOT LUE_FRAMEWORK_BOOLEAN_ELEMENT STREQUAL "std::uint8_t")
            list(APPEND error_messages
                "The PCRaster Python sub-package requires LUE_FRAMEWORK_BOOLEAN_ELEMENT to be std::uint8_t (currently: ${LUE_FRAMEWORK_BOOLEAN_ELEMENT})")
        endif()

        LIST(FIND LUE_FRAMEWORK_INTEGRAL_ELEMENTS "std::int32_t" TYPE_PRESENT)
        if(TYPE_PRESENT EQUAL -1)
            list(APPEND error_messages
                "The PCRaster Python sub-package requires LUE_FRAMEWORK_INTEGRAL_ELEMENTS to contain std::int32_t (currently: ${LUE_FRAMEWORK_INTEGRAL_ELEMENTS})")
        endif()

        LIST(FIND LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS "float" TYPE_PRESENT)
        if(TYPE_PRESENT EQUAL -1)
            list(APPEND error_messages
                "The PCRaster Python sub-package requires LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS to contain float (currently: ${LUE_FRAMEWORK_MATERIAL_ELEMENTS})")
        endif()

        if(error_messages)
            list(APPEND error_messages
                "Set LUE_FRAMEWORK_WITH_PCRASTER_PYTHON_API=FALSE to continue without support for PCRaster")
            list(JOIN error_messages "\n" error_message)
            message(FATAL_ERROR ${error_message})
        endif()
    endif()
endif()


# Handle external dependencies -------------------------------------------------
if(LUE_BUILD_DATA_MODEL)
    set(LUE_HDF5_REQUIRED TRUE)
    set(LUE_BOOST_REQUIRED TRUE)

    if(LUE_DATA_MODEL_WITH_UTILITIES)
        set(LUE_CXXOPTS_REQUIRED TRUE)
        set(LUE_GDAL_REQUIRED TRUE)
        set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
    endif()

    if(LUE_DATA_MODEL_WITH_PYTHON_API)
        set(LUE_PYBIND11_REQUIRED TRUE)
        set(LUE_PYTHON_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_FRAMEWORK)
    set(LUE_BOOST_REQUIRED TRUE)
    set(LUE_GDAL_REQUIRED TRUE)
    set(LUE_HPX_REQUIRED TRUE)
    set(LUE_MDSPAN_REQUIRED TRUE)
    set(LUE_PYTHON_REQUIRED TRUE)  # templatize.py

    if(LUE_FRAMEWORK_WITH_PYTHON_API)
        set(LUE_PYBIND11_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_QUALITY_ASSURANCE)
    set(LUE_NLOHMANN_JSON_REQUIRED TRUE)

    if(LUE_QUALITY_ASSURANCE_WITH_TESTS)
        set(LUE_BUILD_TESTS TRUE)
        set(LUE_BOOST_REQUIRED TRUE)

        if(LUE_BUILD_FRAMEWORK)
            set(LUE_HPXRUN_REQUIRED TRUE)

            # Needed to be able to run hpxrun.py
            set(LUE_PYTHON_REQUIRED TRUE)
        endif()
    endif()

    if(LUE_QUALITY_ASSURANCE_WITH_PYTHON_API)
        set(LUE_PYBIND11_REQUIRED TRUE)
        set(LUE_PYTHON_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_VIEW)
    set(LUE_GLFW_REQUIRED TRUE)
    set(LUE_IMGUI_REQUIRED TRUE)
    set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
endif()


if(LUE_BUILD_DOCUMENTATION)
    # set(LUE_BREATHE_REQUIRED TRUE)
    set(LUE_DOXYGEN_REQUIRED TRUE)
    set(LUE_GRAPHVIZ_REQUIRED TRUE)
    set(LUE_JUPYTER_BOOK_REQUIRED TRUE)
    set(LUE_SPHINX_REQUIRED TRUE)
    set(LUE_DOXYGEN_AWESOME_CSS_REQUIRED TRUE)

    find_program(EDIT_DOT_GRAPH
        edit_dot_graph.py
        PATHS ${PROJECT_SOURCE_DIR}/environment/script
        NO_DEFAULT_PATH
    )
    if(NOT EDIT_DOT_GRAPH)
        message(FATAL_ERROR "edit_dot_graph.py not found")
    endif()
endif()


if(LUE_MDSPAN_REQUIRED)
    FetchContent_Declare(mdspan
        GIT_REPOSITORY https://github.com/kokkos/mdspan.git
        GIT_TAG 9ceface91483775a6c74d06ebf717bbb2768452f  # 0.6.0
        SYSTEM
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(mdspan)
endif()


# ------------------------------------------------------------------------------
if(LUE_PYTHON_REQUIRED)
    # This is the first numpy version supporting Python 3.10
    set(LUE_MIN_NUMPY_VERSION 1.21)
    string(REPLACE "." "_" LUE_NPY_NO_DEPRECATED_API "NPY_${LUE_MIN_NUMPY_VERSION}_API_VERSION")

    # Order matters: Pybind11 must be searched for after Python has been found.
    find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development NumPy)

    set(LUE_PYTHON_FROM_CONDA FALSE)

    if((Python_INTERPRETER_ID STREQUAL "Anaconda") OR (Python_EXECUTABLE MATCHES "^.*conda.*$"))
        set(LUE_PYTHON_FROM_CONDA TRUE)

        # If we use a Conda build environment, then packages should be searched for in the current
        # Conda environment.
        cmake_path(GET Python_EXECUTABLE PARENT_PATH LUE_CONDA_PREFIX)
        cmake_path(GET LUE_CONDA_PREFIX PARENT_PATH LUE_CONDA_PREFIX)
        list(APPEND CMAKE_PREFIX_PATH "${LUE_CONDA_PREFIX}")
    endif()

    message(STATUS "Found Python:")
    message(STATUS "    Interpreter ID        : ${Python_INTERPRETER_ID}")
    message(STATUS "        version           : ${Python_VERSION}")
    message(STATUS "        executable        : ${Python_EXECUTABLE}")
    message(STATUS "        site-arch         : ${Python_SITEARCH}")
    message(STATUS "        site-lib          : ${Python_SITELIB}")
    message(STATUS "    NumPy:")
    message(STATUS "        version           : ${Python_NumPy_VERSION}")
    message(STATUS "        include           : ${Python_NumPy_INCLUDE_DIRS}")
    message(STATUS "    LUE_PYTHON_FROM_CONDA : ${LUE_PYTHON_FROM_CONDA}")

    if(LUE_PYTHON_FROM_CONDA)
        message(STATUS "        prefix            : ${LUE_CONDA_PREFIX}")
    endif()
endif()


if(LUE_PYBIND11_REQUIRED)
    # Order matters: Pybind11 must be searched for after Python has been found.
    # pybind11 ≥ 2.12 is needed for numpy 2.0 support

    FetchContent_Declare(pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG a2e59f0e7065404b44dfe92a28aca47ba1378dc4  # 2.13.6
        SYSTEM
        FIND_PACKAGE_ARGS 2.12 CONFIG
    )
    FetchContent_MakeAvailable(pybind11)

    message(STATUS "Found pybind11:")
    message(STATUS "    pybind11_VERSION      : ${pybind11_VERSION}")
    message(STATUS "    pybind11_VERSION_TYPE : ${pybind11_VERSION_TYPE}")
    message(STATUS "    pybind11_INCLUDE_DIRS : ${pybind11_INCLUDE_DIRS}")
    message(STATUS "    pybind11_INCLUDE_DIR  : ${pybind11_INCLUDE_DIR}")
    message(STATUS "    pybind11_DEFINITIONS  : ${pybind11_DEFINITIONS}")
    message(STATUS "    pybind11_LIBRARIES    : ${pybind11_LIBRARIES}")
    message(STATUS "    pybind11_LIBRARY      : ${pybind11_LIBRARY}")
endif()


if(LUE_BOOST_REQUIRED)
    find_package(Boost 1.70 REQUIRED)

    if(Boost_VERSION VERSION_EQUAL "1.75")
        message(FATAL_ERROR
            "Boost-1.75's safe_numerics library is known to contain a bug:\n"
            "https://github.com/boostorg/safe_numerics/issues/94")
    endif()
endif()


if(LUE_CXXOPTS_REQUIRED)
    FetchContent_Declare(cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG 3bf268481da8208d171d8908e6491459de3651d7  # 3.2.0
        SYSTEM
        FIND_PACKAGE_ARGS 3.2 CONFIG
    )
    FetchContent_MakeAvailable(cxxopts)
endif()


if(LUE_HPX_REQUIRED)
    find_package(HPX 1.11...<1.12 REQUIRED)

    if(HPX_FOUND)
        message(STATUS "Using HPX ${HPX_VERSION} found in ${HPX_PREFIX}")

        # Check whether we are using the same build type as HPX
        if (NOT "${HPX_BUILD_TYPE}" STREQUAL "${CMAKE_BUILD_TYPE}")
            message(WARNING
                "CMAKE_BUILD_TYPE does not match HPX_BUILD_TYPE: "
                "\"${CMAKE_BUILD_TYPE}\" != \"${HPX_BUILD_TYPE}\"\n"
                "ABI compatibility is not guaranteed. Expect link errors.")
        endif()
    endif()

    if(LUE_HPXRUN_REQUIRED)
        find_file(HPXRUN "hpxrun.py"
            HINTS ${HPX_PREFIX}/bin
            REQUIRED
        )
    endif()

    message(STATUS "HPX_WITH_NETWORKING           : ${HPX_WITH_NETWORKING}")
    message(STATUS "HPX_WITH_PARCELPORT_TCP       : ${HPX_WITH_PARCELPORT_TCP}")
    message(STATUS "HPX_WITH_PARCELPORT_MPI       : ${HPX_WITH_PARCELPORT_MPI}")
endif()


if(LUE_IMGUI_REQUIRED)
    find_package(imgui REQUIRED)
    find_package(glfw3 REQUIRED)

    if(LUE_VIEW_USE_VULKAN)
        find_package(Vulkan REQUIRED)
    else()
        find_package(OpenGL REQUIRED)
    endif()
endif()


if(LUE_DOXYGEN_REQUIRED)
    find_package(Doxygen REQUIRED dot)
endif()


if(LUE_DOXYGEN_AWESOME_CSS_REQUIRED)
    FetchContent_Declare(doxygen-awesome-css
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
        GIT_TAG d83219b30fb3778033e24dcf750c075ae29a0271  # v2.4.0
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)

    # Save the location the files were cloned into. This allows us to get the path to doxygen-awesome.css.
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    # # Generate the Doxyfile
    # set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    # set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    # configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
endif()


if(LUE_GDAL_REQUIRED)
    # https://gdal.org/en/stable/development/cmake.html
    find_package(GDAL REQUIRED CONFIG)
    message(STATUS "Found GDAL version ${GDAL_VERSION} in ${GDAL_DIR}")
endif()


if(LUE_GRAPHVIZ_REQUIRED)
    find_package(Graphviz REQUIRED)

    if(GRAPHVIZ_FOUND)
        include(GraphvizMacro)
    endif()
endif()


if(LUE_HDF5_REQUIRED)
    if(DEFINED ENV{CONDA_BUILD})
        set(HDF5_PREFER_PARALLEL FALSE)

        if(APPLE)
            # HDF5's find logic may pick up an HDF5 installation outside of the Conda environment. Guide the
            # logic towards the Conda environment prefix.
            set(HDF5_ROOT $ENV{PREFIX})
        endif()
    else()
        set(HDF5_PREFER_PARALLEL TRUE)
    endif()

    # set(HDF5_FIND_DEBUG TRUE)  # Uncomment to debug HDF5's find logic
    find_package(HDF5 REQUIRED COMPONENTS C)

    if(NOT HDF5_FOUND)
        message(FATAL_ERROR "HDF5 not found")
    elseif(NOT TARGET hdf5::hdf5)
        message(FATAL_ERROR "Target hdf5::hdf5 not available")
    endif()

    # In some cases (Windows + Conda build, ...), HDF5_DEFINITIONS is not set, resulting in undefined symbols
    # at link time. If we're sure the library is shared, add a definition ourselves to make the link succeed.
    if(WIN32)
        if(NOT HDF5_DEFINITIONS)
            string(FIND "${HDF5_C_LIBRARIES}" "shared" idx)
            if(idx GREATER_EQUAL 0)
                message(WARNING "Adding -D H5_BUILT_AS_DYNAMIC_LIB to HDF5_DEFINITIONS ourselves")
                set(HDF5_DEFINITIONS "-D H5_BUILT_AS_DYNAMIC_LIB")
            endif()
        endif()
    endif()

    check_symbol_exists(H5_HAVE_THREADSAFE "hdf5.h" HDF5_IS_THREADSAFE)
    unset(CMAKE_REQUIRED_INCLUDED)

    message(STATUS "HDF5_IS_PARALLEL              : ${HDF5_IS_PARALLEL}")
    message(STATUS "HDF5_IS_THREADSAFE            : ${HDF5_IS_THREADSAFE}")
    message(STATUS "HDF5_DEFINITIONS              : ${HDF5_DEFINITIONS}")
    message(STATUS "HDF5_INCLUDE_DIRS             : ${HDF5_INCLUDE_DIRS}")
    message(STATUS "HDF5_C_LIBRARIES              : ${HDF5_C_LIBRARIES}")
endif()


if(LUE_JUPYTER_BOOK_REQUIRED)
    find_package(JupyterBook REQUIRED)
endif()


if(LUE_NLOHMANN_JSON_REQUIRED)
    FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG 9cca280a4d0ccf0c08f47a99aa71d1b0e52f8d03  # v3.11.3
        SYSTEM
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()


if(LUE_SPHINX_REQUIRED)
    # TODO Find Sphinx Python package.
    include(SphinxDoc)

    if(NOT SPHINX_BUILD_EXECUTABLE OR NOT SPHINX_APIDOC_EXECUTABLE)
        message(FATAL_ERROR "sphinx not found")
    endif()
endif()


# Only allow the user to configure the use of parallel I/O if this is something that is supported by the
# platform. If so, the default is to support parallel I/O.
cmake_dependent_option(LUE_FRAMEWORK_WITH_PARALLEL_IO
    "Use parallel I/O for formats that support it"
    TRUE
    "LUE_BUILD_DATA_MODEL;LUE_HDF5_REQUIRED;HDF5_IS_PARALLEL;LUE_BUILD_FRAMEWORK;LUE_HPX_REQUIRED;HPX_WITH_NETWORKING;HPX_WITH_PARCELPORT_MPI"
    FALSE
)
