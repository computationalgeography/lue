include(CheckCXXSymbolExists)


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
# option(LUE_FRAMEWORK_WITH_MPI
#     "Include support for MPI"
#     FALSE)
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

# Options related to external software used by the project
option(LUE_BUILD_HPX
    "If HPX is required, build it instead of relying on the environment"
    FALSE)
option(LUE_BUILD_OTF2
    "If OTF2 is required, build it instead of relying on the environment"
    FALSE)
option(LUE_OTF2_WITH_PYTHON
    "If OTF2 is built, include the Python bindings"
    FALSE)


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
    if(LUE_DATA_MODEL_WITH_PYTHON_API)
        set(LUE_SPAN_LITE_REQUIRED TRUE)
    endif()

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
    set(LUE_FMT_REQUIRED TRUE)
    set(LUE_BOOST_REQUIRED TRUE)

    if(LUE_BUILD_FRAMEWORK)
        if(DEFINED ENV{CONDA_BUILD})
            set(HDF5_PREFER_PARALLEL FALSE)
        else()
            set(HDF5_PREFER_PARALLEL TRUE)
        endif()
    endif()

    if(LUE_DATA_MODEL_WITH_UTILITIES)
        set(LUE_DOCOPT_REQUIRED TRUE)
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
    set(LUE_DOCOPT_REQUIRED TRUE)
    set(LUE_FMT_REQUIRED TRUE)
    set(LUE_GDAL_REQUIRED TRUE)
    set(LUE_HPX_REQUIRED TRUE)
    set(LUE_MDSPAN_REQUIRED TRUE)
    set(LUE_PYTHON_REQUIRED TRUE)  # templatize.py

    # if(LUE_BUILD_HPX AND LUE_BUILD_OTF2 AND LUE_OTF2_WITH_PYTHON)
    #     set(LUE_PYTHON_REQUIRED TRUE)
    # endif()

    if(LUE_FRAMEWORK_WITH_PYTHON_API)
        set(LUE_PYBIND11_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_QUALITY_ASSURANCE)
    set(LUE_NLOHMANN_JSON_REQUIRED TRUE)

    if(LUE_QUALITY_ASSURANCE_WITH_TESTS)
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
    set(LUE_DOCOPT_REQUIRED TRUE)
    set(LUE_FMT_REQUIRED TRUE)
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
        GIT_TAG 3e9dfa2866941655c56877882565e7577de6fc7b  # 2.12.0
        SYSTEM
        FIND_PACKAGE_ARGS 2.12
    )
    FetchContent_MakeAvailable(pybind11)

    # Silence pybind11 for now
    # https://github.com/computationalgeography/lue/issues/484
    target_compile_definitions(pybind11::module
        INTERFACE
            PYBIND11_NO_ASSERT_GIL_HELD_INCREF_DECREF)
endif()


if(LUE_BOOST_REQUIRED)
    find_package(Boost 1.70 REQUIRED COMPONENTS ${LUE_REQUIRED_BOOST_COMPONENTS})

    if(Boost_VERSION VERSION_EQUAL "1.75")
        message(FATAL_ERROR
            "Boost-1.75's safe_numerics library is known to contain a bug:\n"
            "https://github.com/boostorg/safe_numerics/issues/94")
    endif()
endif()


if(LUE_HPX_REQUIRED)
    if(LUE_BUILD_HPX)
        # Build HPX ourselves
        if(HPX_WITH_APEX)
            if(APEX_WITH_OTF2)
                if(LUE_BUILD_OTF2)

                    set(OTF2_ROOT ${PROJECT_BINARY_DIR}/otf2)

                    set(otf2_version 2.3)
                    set(otf2_patch_file ${CMAKE_CURRENT_SOURCE_DIR}/environment/cmake/otf2-${otf2_version}.patch)

                    if(EXISTS ${otf2_patch_file})
                        set(otf2_patch_command patch src/otf2_archive_int.c ${otf2_patch_file})
                    endif()

                    FetchContent_Declare(otf2
                        URL http://perftools.pages.jsc.fz-juelich.de/cicd/otf2/tags/otf2-${otf2_version}/otf2-${otf2_version}.tar.gz
                        URL_HASH MD5=b85dd4d11e67180e2d5fe66d05112c4b
                        PATCH_COMMAND ${otf2_patch_command}
                        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
                        SYSTEM
                    )

                    FetchContent_GetProperties(otf2)

                    if(NOT otf2_POPULATED)
                        FetchContent_Populate(otf2)

                        set(otf2_system_type
                            "${CMAKE_HOST_SYSTEM_PROCESSOR}-pc-${CMAKE_HOST_SYSTEM_NAME}")
                        string(TOLOWER ${otf2_system_type} otf2_system_type)

                        message(STATUS "Build OTF2")
                        message(STATUS "  otf2_SOURCE_DIR: ${otf2_SOURCE_DIR}")
                        message(STATUS "  otf2_BINARY_DIR: ${otf2_BINARY_DIR}")
                        message(STATUS "  OTF2_ROOT      : ${OTF2_ROOT}")
                        message(STATUS "  system-type    : ${otf2_system_type}")

                        # TODO Use LUE_OTF2_WITH_PYTHON to turn on/off the
                        #   build of the Python bindings.
                        # PYTHON=${Python_EXECUTABLE} PYTHON_FOR_GENERATOR=:
                        execute_process(
                            COMMAND
                                ${otf2_SOURCE_DIR}/configure
                                    --prefix ${OTF2_ROOT}
                                    --build=${otf2_system_type}
                                    --host=${otf2_system_type}
                                    --target=${otf2_system_type}
                                    CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
                                    PYTHON=: PYTHON_FOR_GENERATOR=:
                            WORKING_DIRECTORY
                                ${otf2_BINARY_DIR}
                        )

                        include(ProcessorCount)
                        ProcessorCount(nr_cores)
                        math(EXPR nr_cores_to_use "${nr_cores} / 2")

                        execute_process(
                            COMMAND
                                make -j${nr_cores_to_use}
                            WORKING_DIRECTORY
                                ${otf2_BINARY_DIR}
                        )
                        execute_process(
                            COMMAND
                                make install
                            WORKING_DIRECTORY
                                ${otf2_BINARY_DIR}
                        )
                    endif()
                endif()
            endif()
        endif()

        if(LUE_HPX_GIT_TAG)
            # Below we use GIT_SHALLOW option of FetchContent_Declare, which speeds things up
            # a lot. In case the GIT_TAG is a Git hash, this option cannot be used. It can be
            # used with branch names and tag names.
            string(LENGTH ${LUE_HPX_GIT_TAG} hpx_git_tag_length)

            # TODO Improve this naive test
            if(hpx_git_tag_length EQUAL 11 OR hpx_git_tag_length EQUAL 40)
                set(hpx_git_tag_is_hash TRUE)
            else()
                set(hpx_git_tag_is_hash FALSE)
            endif()

            if(hpx_git_tag_is_hash)
                set(hpx_git_shallow FALSE)
            else()
                set(hpx_git_shallow TRUE)
            endif()

            if(LUE_HPX_REPOSITORY)
                set(hpx_repository "${LUE_HPX_REPOSITORY}")
            else()
                set(hpx_repository "https://github.com/STEllAR-GROUP/hpx")
            endif()

            message(STATUS "Using HPX ${LUE_HPX_GIT_TAG} from repository at ${hpx_repository}")

            FetchContent_Declare(hpx
                GIT_REPOSITORY ${hpx_repository}
                GIT_TAG ${LUE_HPX_GIT_TAG}
                GIT_SHALLOW ${hpx_git_shallow}
                SYSTEM
            )
        else()
            # Obtain HPX from archive. This has the advantage of being
            # able to patch the source files.
            if(LUE_HPX_VERSION)
                # A specific version is requested
                list(APPEND hpx_versions_to_try ${LUE_HPX_VERSION})
            else()
                # Try these versions in turn
                list(APPEND hpx_versions_to_try v1.10.0)
                list(APPEND hpx_versions_to_try v1.9.1)
                list(APPEND hpx_versions_to_try v1.9.0)
            endif()

            # First see if an HPX archive is available in a local cache
            if(LUE_REPOSITORY_CACHE AND EXISTS ${LUE_REPOSITORY_CACHE})
                foreach(hpx_version_ ${hpx_versions_to_try})
                    if(EXISTS "${LUE_REPOSITORY_CACHE}/${hpx_version_}.tar.gz")
                        # Use local archive
                        set(hpx_version ${hpx_version_})  # Loop variables are not available outside a loop
                        set(hpx_url "file://${LUE_REPOSITORY_CACHE}/${hpx_version}.tar.gz")
                        break()
                    endif()
                endforeach()
            endif()

            if(NOT hpx_url)
                # Use remote archive
                list(GET hpx_versions_to_try 0 hpx_version)
                set(hpx_url "https://github.com/STEllAR-GROUP/hpx/archive/${hpx_version}.tar.gz")
            endif()

            set(hpx_patch_file ${CMAKE_CURRENT_SOURCE_DIR}/environment/cmake/hpx-${hpx_version}.patch)

            message(STATUS "Using HPX version ${hpx_version} from archive at ${hpx_url}")

            if(EXISTS ${hpx_patch_file})
                # Get rid of the final warnings in HPX sources
                set(hpx_patch_command git apply --ignore-space-change --ignore-whitespace "${hpx_patch_file}")
                message(STATUS "    Applying patch from ${hpx_patch_file}")
            endif()

            FetchContent_Declare(hpx
                URL ${hpx_url}
                PATCH_COMMAND ${hpx_patch_command}
                DOWNLOAD_EXTRACT_TIMESTAMP FALSE
                SYSTEM
            )
        endif()

        FetchContent_MakeAvailable(hpx)

        if(LUE_HPXRUN_REQUIRED)
            set(HPXRUN "${CMAKE_BINARY_DIR}/_deps/hpx-build/bin/hpxrun.py")
        endif()
    else()
        block(SCOPE_FOR POLICIES)
            cmake_policy(SET CMP0167 OLD)  # Not needed anymore for HPX >= 1.11
            find_package(HPX 1.10...<1.11 REQUIRED)
        endblock()

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
    endif()

    if(LUE_HPXRUN_REQUIRED)
        find_file(HPXRUN "hpxrun.py"
            HINTS ${HPX_PREFIX}
        )

    #      if(LUE_HPX_REQUIRED AND LUE_BUILD_HPX)
    #             set(HPXRUN "${CMAKE_BINARY_DIR}/_deps/hpx-build/bin/hpxrun.py")
    #         else()
    #             find_file(HPXRUN "hpxrun.py")

    #             if(NOT HPXRUN)
    #                 message(FATAL_ERROR "hpxrun.py not found")
    #             endif()
    #         endif()
    endif()
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


if(LUE_DOCOPT_REQUIRED)
    FetchContent_Declare(docopt
        GIT_REPOSITORY https://github.com/docopt/docopt.cpp.git
        GIT_TAG 42ebcec9dc2c99a1b3a4542787572045763ad196  # 0.6.3
        SYSTEM
        # Using the system version is inconvenient since headers are then in a docopt/ subdirectory...
        # FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(docopt)

    # The docopt package shipѕ with CMake scripts that define a target named docopt (shared
    # library) and docopt_s (static library).
endif()


if(LUE_DOXYGEN_REQUIRED)
    find_package(Doxygen REQUIRED dot)
endif()


if(LUE_GDAL_REQUIRED)
    find_package(GDAL REQUIRED)
endif()


if(LUE_GRAPHVIZ_REQUIRED)
    find_package(Graphviz REQUIRED)

    if(GRAPHVIZ_FOUND)
        include(GraphvizMacro)
    endif()
endif()

if(LUE_FMT_REQUIRED)
    # In fmt, install rules are disabled if it's a subproject
    set(FMT_INSTALL ON)
    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281  # 10.2.1
        SYSTEM
        FIND_PACKAGE_ARGS 10
    )
    FetchContent_MakeAvailable(fmt)
endif()


if(LUE_HDF5_REQUIRED)
    # Explicitly use Module Mode, to prevent the use of HDF5's own CMake find logic. This latter
    # logic does not provide us with the hdf5::hdf5 target, which CMake's module defines.
    # Note that Conan prefers Config Mode (it sets CMAKE_FIND_PACKAGE_PREFER_CONFIG in the
    # toolchain).
    find_package(HDF5 MODULE REQUIRED COMPONENTS C)
    message(STATUS "HDF5_IS_PARALLEL              : ${HDF5_IS_PARALLEL}")
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

if(LUE_SPAN_LITE_REQUIRED)
    FetchContent_Declare(span-lite
        GIT_REPOSITORY https://github.com/martinmoene/span-lite.git
        GIT_TAG cbb9c3c5162a0d7018c7b3e053153a04d4fbbbb9  # 0.10.3
        SYSTEM
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(span-lite)
endif()

if(LUE_SPHINX_REQUIRED)
    # TODO Find Sphinx Python package.
    include(SphinxDoc)

    if(NOT SPHINX_BUILD_EXECUTABLE OR NOT SPHINX_APIDOC_EXECUTABLE)
        message(FATAL_ERROR "sphinx not found")
    endif()
endif()
