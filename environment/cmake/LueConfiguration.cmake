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
option(LUE_FRAMEWORK_WITH_PYTHON_API
    "Include Python API for modelling framework"
    FALSE)

option(LUE_BUILD_VIEW
    "Build LUE data model viewer"
    FALSE)

option(LUE_BUILD_DOCUMENTATION
    "Build documentation"
    FALSE)

option(LUE_BUILD_TEST
    "Build tests"
    FALSE)
set(LUE_TEST_NR_LOCALITIES_PER_TEST
    1 CACHE STRING "Number of localities to use")
set(LUE_TEST_NR_THREADS_PER_LOCALITY
    1 CACHE STRING "Number of threads to use")
set(LUE_TEST_HPX_RUNWRAPPER
    none CACHE STRING "Which runwrapper to use (see hpxrun.py)")
set(LUE_TEST_HPX_PARCELPORT
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
# TODO Make the defaults dependent on the result of a search by CMake.
if(WIN32)
    set(LUE_HAVE_BOOST_INIT FALSE)
    set(LUE_HAVE_DOCOPT_INIT FALSE)
    set(LUE_HAVE_DOXYGEN_INIT FALSE)
    set(LUE_HAVE_GDAL_INIT FALSE)
    set(LUE_HAVE_GLEW_INIT FALSE)
    set(LUE_HAVE_GLFW_INIT FALSE)
    set(LUE_HAVE_FMT_INIT FALSE)
    set(LUE_HAVE_HDF5_INIT FALSE)
    set(LUE_HAVE_NLOHMANN_JSON_INIT FALSE)
    set(LUE_HAVE_PYBIND11_INIT FALSE)
elseif(APPLE)
    set(LUE_HAVE_BOOST_INIT TRUE)
    # Change default to TRUE once Brew contains a version.
    # Change default to TRUE once package integrates well.
    set(LUE_HAVE_DOCOPT_INIT FALSE)
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
    set(LUE_HAVE_GDAL_INIT TRUE)
    set(LUE_HAVE_GLEW_INIT TRUE)
    # Change default to TRUE once package integrates well.
    set(LUE_HAVE_GLFW_INIT FALSE)
    set(LUE_HAVE_FMT_INIT TRUE)
    set(LUE_HAVE_HDF5_INIT TRUE)
    set(LUE_HAVE_NLOHMANN_JSON_INIT TRUE)
    # Change default to TRUE once Ubuntu LTS contains a recent enough version.
    set(LUE_HAVE_PYBIND11_INIT FALSE)
else()
    set(LUE_HAVE_BOOST_INIT TRUE)
    # Change default to TRUE once package integrates well.
    set(LUE_HAVE_DOCOPT_INIT FALSE)
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
    set(LUE_HAVE_GDAL_INIT TRUE)
    set(LUE_HAVE_GLEW_INIT TRUE)
    # Change default to TRUE once package integrates well.
    set(LUE_HAVE_GLFW_INIT FALSE)
    set(LUE_HAVE_FMT_INIT TRUE)
    set(LUE_HAVE_HDF5_INIT TRUE)
    # Change default to TRUE once Ubuntu LTS contains a recent enough version.
    set(LUE_HAVE_NLOHMANN_JSON_INIT FALSE)
    # Change default to TRUE once Ubuntu LTS contains a recent enough version.
    set(LUE_HAVE_PYBIND11_INIT FALSE)
endif()

function(lue_have_option name)
    option(LUE_HAVE_${name}
        "If ${name} is required, assume it is installed"
        ${LUE_HAVE_${name}_INIT})
endfunction()

lue_have_option(BOOST)
lue_have_option(DOCOPT)
lue_have_option(DOXYGEN)
lue_have_option(GDAL)
lue_have_option(GLEW)
lue_have_option(GLFW)
lue_have_option(FMT)
lue_have_option(HDF5)
lue_have_option(NLOHMANN_JSON)
lue_have_option(PYBIND11)


# Handle internal dependencies -------------------------------------------------
if(LUE_BUILD_VIEW)
    set(LUE_BUILD_DATA_MODEL TRUE)
    set(LUE_BUILD_IMGUI TRUE)
endif()


if(LUE_BUILD_DATA_MODEL)
    if(LUE_DATA_MODEL_WITH_PYTHON_API)
    endif()
endif()


if(LUE_BUILD_FRAMEWORK)
    if(LUE_FRAMEWORK_WITH_DASHBOARD)
        set(LUE_BUILD_IMGUI TRUE)
    endif()
endif()


if(LUE_BUILD_TEST)
endif()


if(LUE_BUILD_DOCUMENTATION)
endif()


# Handle external dependencies -------------------------------------------------
if(LUE_BUILD_DATA_MODEL)
    set(LUE_HDF5_REQUIRED TRUE)
    # list(APPEND LUE_REQUIRED_HDF5_COMPONENTS C)  # HL
    set(LUE_FMT_REQUIRED TRUE)

    set(LUE_BOOST_REQUIRED TRUE)
    list(APPEND LUE_REQUIRED_BOOST_COMPONENTS
        filesystem system)

    if(LUE_BUILD_FRAMEWORK)
        set(HDF5_PREFER_PARALLEL TRUE)
    endif()

    if(LUE_DATA_MODEL_WITH_UTILITIES)
        set(LUE_DOCOPT_REQUIRED TRUE)
        set(LUE_GDAL_REQUIRED TRUE)
        set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
    endif()

    if(LUE_DATA_MODEL_WITH_PYTHON_API)
        set(LUE_GUIDELINE_SUPPORT_LIBRARY_REQUIRED TRUE)
        set(LUE_PYBIND11_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_FRAMEWORK)
    set(LUE_BOOST_REQUIRED TRUE)
    set(LUE_DOCOPT_REQUIRED TRUE)
    set(LUE_FMT_REQUIRED TRUE)
    set(LUE_HPX_REQUIRED TRUE)
    set(LUE_KOKKOS_MDSPAN_REQUIRED TRUE)

    if(LUE_BUILD_HPX AND LUE_BUILD_OTF2 AND LUE_OTF2_WITH_PYTHON)
        set(LUE_PYTHON_REQUIRED TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_OPENCL)
        set(LUE_OPENCL_REQUIRED TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_DASHBOARD)
        set(LUE_IMGUI_REQUIRED TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_BENCHMARKS)
        set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
    endif()

    if(LUE_FRAMEWORK_WITH_PYTHON_API)
        set(LUE_PYBIND11_REQUIRED TRUE)
    endif()
endif()


if(LUE_BUILD_TEST)
    set(LUE_BOOST_REQUIRED TRUE)
    list(APPEND LUE_REQUIRED_BOOST_COMPONENTS
        filesystem system unit_test_framework)

    if(LUE_BUILD_FRAMEWORK)
        set(HPXRUN "${CMAKE_BINARY_DIR}/_deps/hpx-build/bin/hpxrun.py")

        # Needed to be able to run hpxrun.py
        set(LUE_PYTHON_REQUIRED TRUE)

        # Does not work when HPX is built as part of LUE build
        # find_file(HPXRUN
        #     "hpxrun.py"
        #     PATHS ${CMAKE_BINARY_DIR}/_deps/hpx-build/bin)
        # 
        # if(NOT HPXRUN)
        #     message(FATAL_ERROR "hpxrun.py not found")
        # endif()
    endif()
endif()


if(LUE_BUILD_VIEW)
    set(LUE_DOCOPT_REQUIRED TRUE)
    set(LUE_FMT_REQUIRED TRUE)
    set(LUE_GLEW_REQUIRED TRUE)
    set(LUE_GLFW_REQUIRED TRUE)
    set(LUE_IMGUI_REQUIRED TRUE)
    set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
endif()


if(LUE_BUILD_DOCUMENTATION)
    # set(LUE_BREATHE_REQUIRED TRUE)
    set(LUE_DOXYGEN_REQUIRED TRUE)
    set(LUE_GRAPHVIZ_REQUIRED TRUE)
    set(LUE_SPHINX_REQUIRED TRUE)
    set(LUE_SPHINX_LUE_THEME_REQUIRED TRUE)

    find_program(EDIT_DOT_GRAPH
        edit_dot_graph.py
        PATHS ${PROJECT_SOURCE_DIR}/environment/script
        NO_DEFAULT_PATH
    )
    if(NOT EDIT_DOT_GRAPH)
        message(FATAL_ERROR "edit_dot_graph.py not found")
    endif()

    find_package(LATEX)

    if(NOT LATEX_FOUND)
        message(WARNING
            "LaTeX could not be found. Latex documents will not be generated")
    else()
        include(UseLATEX)
    endif()
endif()


# ------------------------------------------------------------------------------
# Packages that need to be installed with Conan must be installed first,
# so packages using them will find them

if(LUE_BOOST_REQUIRED)
    # Disable search for boost-cmake stuff
    set(Boost_NO_BOOST_CMAKE TRUE)

    if(NOT LUE_HAVE_BOOST)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} boost/1.75.0)
        set(LUE_CONAN_OPTIONS ${LUE_CONAN_OPTIONS} boost:shared=True)
    endif()
endif()

if(LUE_DOCOPT_REQUIRED)
    if(NOT LUE_HAVE_DOCOPT)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} docopt.cpp/0.6.3)
    endif()
endif()

if(LUE_FMT_REQUIRED)
    if(NOT LUE_HAVE_FMT)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} fmt/8.0.0)
    endif()
endif()

if(LUE_GDAL_REQUIRED)
    if(NOT LUE_HAVE_GDAL)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} gdal/3.2.1)
    endif()
endif()


if(LUE_GLEW_REQUIRED)
    if(NOT LUE_HAVE_GLEW)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} glew/2.2.0)
    endif()
endif()


if(LUE_GLFW_REQUIRED)
    if(NOT LUE_HAVE_GLFW)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} glfw/3.3.4)
    endif()
endif()


if(LUE_GUIDELINE_SUPPORT_LIBRARY_REQUIRED)
    set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} ms-gsl/3.1.0)
endif()

if(LUE_HDF5_REQUIRED)
    if(NOT LUE_HAVE_HDF5)
        set(HDF5_VERSION 1.12.0)
        set(HDF5_IS_PARALLEL FALSE)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} hdf5/${HDF5_VERSION})
    endif()
endif()

if(LUE_HPX_REQUIRED)
    if(NOT LUE_BUILD_HPX)
        message(FATAL_ERROR
            "Support for system-provided HPX library does not work yet\n"
            "But we can build HPX for you! Just reconfigure with LUE_BUILD_HPX=TRUE")

        # # Use HPX from the environment
        # find_package(HPX REQUIRED)

        # if(HPX_FOUND)
        #     message(STATUS "Found HPX")
        #     message(STATUS "  includes : ${HPX_INCLUDE_DIRS}")
        #     message(STATUS "  libraries: ${HPX_LIBRARIES}")

        #     # Check whether we are using the same build type as HPX
        #     if (NOT "${HPX_BUILD_TYPE}" STREQUAL "${CMAKE_BUILD_TYPE}")
        #         message(WARNING
        #             "CMAKE_BUILD_TYPE does not match HPX_BUILD_TYPE: "
        #             "\"${CMAKE_BUILD_TYPE}\" != \"${HPX_BUILD_TYPE}\"\n"
        #             "ABI compatibility is not guaranteed. Expect link errors.")
        #     endif()
        # endif()
    endif()
endif()

if(LUE_IMGUI_REQUIRED AND NOT LUE_BUILD_IMGUI)
    message(FATAL_ERROR
        "Support for system-provided ImGUI library does not work yet\n"
        "But we can build ImGUI for you! Just reconfigure with LUE_BUILD_IMGUI=TRUE")
endif()

if(LUE_KOKKOS_MDSPAN_REQUIRED)
    FetchContent_Declare(kokkos_mdspan
        GIT_REPOSITORY https://github.com/kokkos/mdspan.git
        GIT_TAG a7990884f090365787a90cdc12e689822d642c65  # 20191010
    )
    FetchContent_MakeAvailable(kokkos_mdspan)
endif()

if(LUE_NLOHMANN_JSON_REQUIRED)
    if(NOT LUE_HAVE_NLOHMANN_JSON)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} nlohmann_json/3.9.1)
    endif()
endif()

if(LUE_PYBIND11_REQUIRED)
    if(NOT LUE_HAVE_PYBIND11)
        FetchContent_Declare(pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11
            GIT_TAG "v2.6.2"
        )
        FetchContent_MakeAvailable(pybind11)
    endif()
endif()


include(conan-0.16.1)
conan_cmake_configure(
    REQUIRES ${LUE_CONAN_REQUIRES}
    GENERATORS cmake_find_package
    OPTIONS ${LUE_CONAN_OPTIONS})
conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conan-center
    SETTINGS ${settings})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_BINARY_DIR})


# ------------------------------------------------------------------------------
if(LUE_PYBIND11_REQUIRED)

    # For some reason, PYTHON_EXECUTABLE is set (FetchContent?).
    set(LUE_PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})

    # Ask Python for some properties we can use.
    # Given Python found, figure out where the NumPy headers are. We don't
    # want to pick up headers from another prefix than the prefix of the
    # Python interpreter.
    execute_process(COMMAND "${LUE_PYTHON_EXECUTABLE}" -c "
from distutils import sysconfig
import numpy as np
import sys
print(\"{};{};{};{};{};{}\".format(
        sys.version_info[0], sys.version_info[1], sys.version_info[2],
        sysconfig.get_python_lib(plat_specific=True),
        np.__version__, np.get_include(),
    ))"
        RESULT_VARIABLE python_result
        OUTPUT_VARIABLE python_output
        ERROR_VARIABLE python_error
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT python_result MATCHES 0)
        message(FATAL_ERROR
            "${LUE_PYTHON_EXECUTABLE} is unable to determine interpreter properties:\n${python_error}")
    else()
        list(GET python_output 0 PYTHON_VERSION_MAJOR)
        list(GET python_output 1 PYTHON_VERSION_MINOR)
        list(GET python_output 2 PYTHON_VERSION_PATCH)
        list(GET python_output 3 python_site_packages)
        list(GET python_output 4 numpy_version)
        list(GET python_output 5 NUMPY_INCLUDE_DIRS)
        set(PYTHON_VERSION "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}.${PYTHON_VERSION_PATCH}")

        message(STATUS "Found Python ${PYTHON_VERSION}")
        message(STATUS "Found NumPy ${numpy_version} headers in ${NUMPY_INCLUDE_DIRS}")
    endif()

    if(NOT LUE_PYTHON_API_INSTALL_DIR)
        # Most Python packages install in a subdirectory of Python's site
        # packages. But we currently ship only Python packages implemented
        # as shared libraries. Therefore, we install in the root of the
        # site packages directory. We may have to change things in
        # the future if this is unconventional.
        set(LUE_PYTHON_API_INSTALL_DIR "${python_site_packages}")  # /lue")
    endif()
endif()

if(LUE_PYTHON_REQUIRED AND NOT LUE_PYBIND11_REQUIRED)
    find_package(Python COMPONENTS Interpreter)

    if(NOT Python_FOUND)
        message(FATAL_ERROR "Python not found")
    endif()

    set(LUE_PYTHON_EXECUTABLE ${Python_EXECUTABLE})
endif()

if(LUE_HPX_REQUIRED)
    if(HPX_WITH_APEX)
        if(APEX_WITH_OTF2)
            if(LUE_BUILD_OTF2)

                set(OTF2_ROOT ${PROJECT_BINARY_DIR}/otf2)

                FetchContent_Declare(otf2
                    URL https://www.vi-hps.org/cms/upload/packages/otf2/otf2-2.2.tar.gz
                    URL_HASH MD5=cfedf583bf000e98ce0c86e663e5ded0
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
                    # PYTHON=${LUE_PYTHON_EXECUTABLE} PYTHON_FOR_GENERATOR=:
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

    if(LUE_BUILD_HPX)
        # Build HPX ourselves

        if(LUE_HPX_GIT_TAG)
            # Obtain HPX from GIT repository. This is useful when we
            # need to use a specific HPX commit.
            if(LUE_REPOSITORY_CACHE AND EXISTS "${LUE_REPOSITORY_CACHE}/hpx")
                # Use local repository
                set(hpx_repository "${LUE_REPOSITORY_CACHE}/hpx")
            else()
                # Use remote repository
                set(hpx_repository "https://github.com/STEllAR-GROUP/hpx")
            endif()

            FetchContent_Declare(hpx
                GIT_REPOSITORY ${hpx_repository}
                GIT_TAG ${LUE_HPX_GIT_TAG}
                GIT_SHALLOW ON  # This implies that a commit hash is not allowed
            )
        else()
            # Obtain HPX from archive. This has the advantage of being
            # able to patch the source files.
            set(hpx_version 1.7.0)

            if(LUE_REPOSITORY_CACHE AND EXISTS ${LUE_REPOSITORY_CACHE})
                # Use local archive
                set(hpx_url "file://${LUE_REPOSITORY_CACHE}/${hpx_version}.tar.gz")
            else()
                # Use remote archive
                set(hpx_url "https://github.com/STEllAR-GROUP/hpx/archive/${hpx_version}.tar.gz")
            endif()

            set(hpx_patch_file ${CMAKE_CURRENT_SOURCE_DIR}/environment/cmake/hpx-${hpx_version}.patch)

            if(EXISTS ${hpx_patch_file})
                # Get rid of the final warnings in HPX sources
                set(hpx_patch_command
                    git apply --reject --ignore-space-change --ignore-whitespace ${hpx_patch_file})
            endif()

            FetchContent_Declare(hpx
                URL ${hpx_url}
                PATCH_COMMAND ${hpx_patch_command}
            )
        endif()

        FetchContent_MakeAvailable(hpx)
    endif()
endif()


if(LUE_IMGUI_REQUIRED AND LUE_BUILD_IMGUI)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)

    if(LUE_REPOSITORY_CACHE AND EXISTS ${LUE_REPOSITORY_CACHE}/imgui)
        set(imgui_repository ${LUE_REPOSITORY_CACHE}/imgui)
    else()
        set(imgui_repository https://github.com/ocornut/imgui.git)
    endif()

    FetchContent_Declare(imgui
        // MIT License, see ${imgui_SOURCE_DIR}/LICENSE.txt
        GIT_REPOSITORY ${imgui_repository}
        GIT_TAG v1.81
    )

    FetchContent_GetProperties(imgui)

    if(NOT imgui_POPULATED)
        FetchContent_Populate(imgui)

        add_library(imgui STATIC
            # imgui release
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp

            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        )

        target_include_directories(imgui SYSTEM
            PRIVATE
                ${imgui_SOURCE_DIR}
            PUBLIC
                ${imgui_SOURCE_DIR}/backends
                $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
        )

        target_compile_definitions(imgui
            PUBLIC
                IMGUI_DISABLE_OBSOLETE_FUNCTIONS
                IMGUI_IMPL_OPENGL_LOADER_GLEW
        )

        target_link_libraries(imgui
            PUBLIC
                glfw::glfw
                GLEW::GLEW
                OpenGL::GL
        )

        add_library(imgui::imgui ALIAS imgui)
    endif()
endif()


if(LUE_SPHINX_LUE_THEME_REQUIRED)
    if(LUE_REPOSITORY_CACHE AND EXISTS "${LUE_REPOSITORY_CACHE}/sphinx_rtd_theme")
        set(sphinx_rtd_theme_repository "${LUE_REPOSITORY_CACHE}/sphinx_rtd_theme")
    else()
        set(sphinx_rtd_theme_repository "https://github.com/computationalgeography/sphinx_rtd_theme")
    endif()

    FetchContent_Declare(sphinx_lue_theme
        GIT_REPOSITORY ${sphinx_rtd_theme_repository}
        GIT_TAG 3ecd7ff
    )

    FetchContent_MakeAvailable(sphinx_lue_theme)
endif()


if(LUE_BOOST_REQUIRED)
    find_package(Boost REQUIRED COMPONENTS ${LUE_REQUIRED_BOOST_COMPONENTS})

    add_definitions(
            -DBOOST_ALL_NO_LIB
            -DBOOST_ALL_DYN_LINK
        )
endif()


if(LUE_DOCOPT_REQUIRED)
    find_package(docopt REQUIRED)
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
    find_package(fmt REQUIRED)
endif()


if(LUE_HDF5_REQUIRED)
    find_package(HDF5 REQUIRED COMPONENTS C)

    if(NOT LUE_HAVE_HDF5)
        # Conan find module uses uppercase target names...
        add_library(hdf5::hdf5 ALIAS HDF5::HDF5)
    endif()
endif()

if(LUE_GUIDELINE_SUPPORT_LIBRARY_REQUIRED)
    find_package(Microsoft.GSL REQUIRED)
endif()

if(LUE_NLOHMANN_JSON_REQUIRED)
    find_package(nlohmann_json REQUIRED)
endif()

if(LUE_OPENCL_REQUIRED)
    find_package(OpenCL REQUIRED)
endif()

if(LUE_SPHINX_REQUIRED)
    # TODO Find Sphinx Python package.
    include(SphinxDoc)

    if(NOT SPHINX_BUILD_EXECUTABLE OR NOT SPHINX_APIDOC_EXECUTABLE)
        message(FATAL_ERROR "sphinx not found")
    endif()
endif()
