include(CheckCXXSymbolExists)


# Options to be set by the user ------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
    option(LUE_ENABLE_IWYU
        "Enable include-what-you-use"
        FALSE)

    if(LUE_ENABLE_IWYU)
        set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

        find_program(INCLUDE_WHAT_YOU_USE_EXECUTABLE include-what-you-use REQUIRED)

        # Revisit once it is easy to use include-what-you-use >= 0.18 (try: include-what-you-use --version)
        # set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
        #     ${INCLUDE_WHAT_YOU_USE_EXECUTABLE}
        #     -Xiwyu --mapping-file=...
        #     -Xiwyu --error
        #     CACHE STRING "Include-what-you-use command"
        # )
    endif()
endif()


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
option(LUE_FRAMEWORK_WITH_IMAGE_LAND
    "Include operations required for the IMAGE land-use allocation model"
    FALSE)

option(LUE_BUILD_VIEW
    "Build LUE data model viewer"
    TRUE)

option(LUE_BUILD_DOCUMENTATION
    "Build documentation"
    FALSE)

option(LUE_BUILD_QA
    "Include support for QA"
    FALSE)
option(LUE_QA_WITH_PYTHON_API
    "Include Python API for QA"
    FALSE)
option(LUE_QA_WITH_TESTS
    "Include tests"
    FALSE)
set(LUE_QA_TEST_NR_LOCALITIES_PER_TEST
    1 CACHE STRING "Number of localities to use")
set(LUE_QA_TEST_NR_THREADS_PER_LOCALITY
    1 CACHE STRING "Number of threads to use")
set(LUE_QA_TEST_HPX_RUNWRAPPER
    none CACHE STRING "Which runwrapper to use (see hpxrun.py)")
set(LUE_QA_TEST_HPX_PARCELPORT
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


if(LUE_BUILD_QA)
    if(LUE_QA_WITH_TESTS)
    endif()
endif()


if(LUE_BUILD_DOCUMENTATION)
endif()


set(LUE_TEMPLATIZE "${PROJECT_SOURCE_DIR}/environment/script/templatize.py")

# NOTE These can be made configurable later on
set(LUE_FRAMEWORK_CONDITION_ELEMENT uint8_t)
set(LUE_FRAMEWORK_BOOLEAN_ELEMENT uint8_t)
set(LUE_FRAMEWORK_INDEX_ELEMENT uint64_t)
set(LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT uint8_t)
set(LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS int32_t int64_t)
set(LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS uint8_t uint32_t uint64_t)
set(LUE_FRAMEWORK_INTEGRAL_ELEMENTS
    ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS}
    ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})
set(LUE_FRAMEWORK_ZONE_ELEMENTS ${LUE_FRAMEWORK_INTEGRAL_ELEMENTS})
set(LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS float double)
set(LUE_FRAMEWORK_ELEMENTS
    ${LUE_FRAMEWORK_INTEGRAL_ELEMENTS}
    ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})


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


if(LUE_BUILD_QA)
    set(LUE_NLOHMANN_JSON_REQUIRED TRUE)

    if(LUE_QA_WITH_TESTS)
        set(LUE_BOOST_REQUIRED TRUE)
        list(APPEND LUE_REQUIRED_BOOST_COMPONENTS
            filesystem system unit_test_framework)

        if(LUE_BUILD_FRAMEWORK)
            set(LUE_HPXRUN_REQUIRED TRUE)

            # Needed to be able to run hpxrun.py
            set(LUE_PYTHON_REQUIRED TRUE)
        endif()
    endif()

    if(LUE_QA_WITH_PYTHON_API)
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
    set(LUE_SPHINX_REQUIRED TRUE)

    find_program(EDIT_DOT_GRAPH
        edit_dot_graph.py
        PATHS ${PROJECT_SOURCE_DIR}/environment/script
        NO_DEFAULT_PATH
    )
    if(NOT EDIT_DOT_GRAPH)
        message(FATAL_ERROR "edit_dot_graph.py not found")
    endif()

    # find_package(LATEX)

    # if(NOT LATEX_FOUND)
    #     message(WARNING
    #         "LaTeX could not be found. Latex documents will not be generated")
    # else()
    #     include(UseLATEX)
    # endif()
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
    # This is the first numpy version supporting Python 3.9
    set(LUE_MIN_NUMPY_VERSION 1.19)
    string(REPLACE "." "_" LUE_NPY_NO_DEPRECATED_API "NPY_${LUE_MIN_NUMPY_VERSION}_API_VERSION")

    # Order matters: Pybind11 must be searched for after Python has been found.
    find_package(Python 3.9 REQUIRED COMPONENTS Interpreter Development NumPy)

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

    # Disable support for numpy 2.0 for now
    # https://github.com/computationalgeography/lue/issues/664
    target_compile_definitions(pybind11::module
        INTERFACE
            PYBIND11_NUMPY_1_ONLY)

    # Silence pybind11 for now
    # https://github.com/computationalgeography/lue/issues/484
    target_compile_definitions(pybind11::module
        INTERFACE
            PYBIND11_NO_ASSERT_GIL_HELD_INCREF_DECREF)
endif()


if(LUE_BOOST_REQUIRED)
    if(LUE_HPX_REQUIRED AND LUE_BUILD_HPX)
        # HPX inspect tool requires Boost.Regex
        list(APPEND LUE_REQUIRED_BOOST_COMPONENTS regex)
    endif()

    set(Boost_NO_BOOST_CMAKE TRUE)

    find_package(Boost REQUIRED COMPONENTS ${LUE_REQUIRED_BOOST_COMPONENTS})

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
                        set(hpx_version ${hpx_version_})  # Loop veriables are not available outside a loop
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
        find_package(HPX REQUIRED)

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
        FIND_PACKAGE_ARGS
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
