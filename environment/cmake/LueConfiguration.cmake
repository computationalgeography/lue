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
    set(LUE_HAVE_MS_GSL_INIT FALSE)
    set(LUE_HAVE_FMT_INIT FALSE)
    set(LUE_HAVE_HDF5_INIT FALSE)
    set(LUE_HAVE_NETCDF4_INIT FALSE)
    set(LUE_HAVE_NLOHMANN_JSON_INIT FALSE)
    set(LUE_HAVE_PYBIND11_INIT FALSE)
elseif(APPLE)
    set(LUE_HAVE_BOOST_INIT TRUE)
    # Change default to TRUE once Brew contains a version.
    set(LUE_HAVE_DOCOPT_INIT FALSE)
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
    set(LUE_HAVE_GDAL_INIT TRUE)
    set(LUE_HAVE_GLEW_INIT TRUE)
    set(LUE_HAVE_GLFW_INIT TRUE)
    set(LUE_HAVE_MS_GSL_INIT FALSE)
    set(LUE_HAVE_FMT_INIT TRUE)
    set(LUE_HAVE_HDF5_INIT TRUE)
    set(LUE_HAVE_NETCDF4_INIT TRUE)
    set(LUE_HAVE_NLOHMANN_JSON_INIT TRUE)
    set(LUE_HAVE_PYBIND11_INIT FALSE)
else()
    set(LUE_HAVE_BOOST_INIT TRUE)
    set(LUE_HAVE_DOCOPT_INIT TRUE)
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
    set(LUE_HAVE_GDAL_INIT TRUE)
    set(LUE_HAVE_GLEW_INIT TRUE)
    set(LUE_HAVE_GLFW_INIT TRUE)
    set(LUE_HAVE_MS_GSL_INIT FALSE)
    set(LUE_HAVE_FMT_INIT TRUE)
    set(LUE_HAVE_HDF5_INIT TRUE)
    set(LUE_HAVE_NETCDF4_INIT TRUE)
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
lue_have_option(MS_GSL)
lue_have_option(HDF5)
lue_have_option(NETCDF4)
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
        set(LUE_NETCDF4_REQUIRED TRUE)
        set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
    endif()

    if(LUE_DATA_MODEL_WITH_PYTHON_API)
        set(LUE_MS_GSL_REQUIRED TRUE)
        set(LUE_PYBIND11_REQUIRED TRUE)
        set(LUE_PYTHON_REQUIRED TRUE)
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
        set(LUE_PYTHON_REQUIRED TRUE)
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
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} boost/1.78.0)
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
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} fmt/8.1.1)
    endif()
endif()

if(LUE_GDAL_REQUIRED)
    if(NOT LUE_HAVE_GDAL)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} gdal/3.4.1)
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


if(LUE_MS_GSL_REQUIRED)
    if(NOT LUE_HAVE_MS_GSL)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} ms-gsl/4.0.0)
    endif()
endif()

if(LUE_HDF5_REQUIRED)
    if(NOT LUE_HAVE_HDF5)
        # NOTE: Keep this in sync with Conan's version of NetCDF4
        set(HDF5_VERSION 1.12.1)
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

if(LUE_NETCDF4_REQUIRED)
    if(NOT LUE_HAVE_NETCDF4)
        # NOTE: Keep this in sync with Conan's version of HDF5
        set(NETCDF4_VERSION 4.7.4)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} hdf5/${NETCDF4_VERSION})
    endif()
endif()

if(LUE_NLOHMANN_JSON_REQUIRED)
    if(NOT LUE_HAVE_NLOHMANN_JSON)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} nlohmann_json/3.10.5)
    endif()
endif()


include(conan-0.17.0)
conan_add_remote(
    NAME conancenter
    URL https://center.conan.io
    VERIFY_SSL False)
conan_cmake_configure(
    REQUIRES ${LUE_CONAN_REQUIRES}
    GENERATORS cmake_find_package
    OPTIONS ${LUE_CONAN_OPTIONS})
conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings})
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_BINARY_DIR})


# ------------------------------------------------------------------------------
if(LUE_PYTHON_REQUIRED)
    # Order matters: Pybind11 must be searched for after Python has been found.
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development NumPy)
endif()


if(LUE_PYBIND11_REQUIRED)
    # Order matters: Pybind11 must be searched for after Python has been found.
    if(NOT LUE_HAVE_PYBIND11)
        FetchContent_Declare(pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11
            GIT_TAG "v2.8.1"
        )

        # This should pick up the Python found above
        FetchContent_MakeAvailable(pybind11)
    else()
        find_package(pybind11 REQUIRED)
    endif()

    if(NOT LUE_PYTHON_API_INSTALL_DIR)
        # Most Python packages install in a subdirectory of Python's site
        # packages. But we currently ship only Python packages implemented
        # as shared libraries. Therefore, we install in the root of the
        # site packages directory. We may have to change things in
        # the future if this is unconventional.
        set(LUE_PYTHON_API_INSTALL_DIR "${Python3_SITEARCH}")  # /lue")
    endif()
endif()


if(LUE_BOOST_REQUIRED)
    if(LUE_HPX_REQUIRED AND LUE_BUILD_HPX)
        # HPX inspect tool requires Boost.Regex
        list(APPEND LUE_REQUIRED_BOOST_COMPONENTS regex)
    endif()

    find_package(Boost REQUIRED COMPONENTS ${LUE_REQUIRED_BOOST_COMPONENTS})

    add_definitions(
            -DBOOST_ALL_NO_LIB
            -DBOOST_ALL_DYN_LINK
        )

    if(NOT LUE_HAVE_BOOST)
        # Boost provided by Conan. Unit tests need path to dlls to be set. In the unit test
        # macro's this variable is used to locate them.
        cmake_path(GET Boost_INCLUDE_DIR PARENT_PATH Boost_LIBRARY_DIRS)
        set(Boost_LIBRARY_DIRS "${Boost_LIBRARY_DIRS}/bin")
    endif()
endif()


if(LUE_HPX_REQUIRED)
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
                    # PYTHON=${Python3_EXECUTABLE} PYTHON_FOR_GENERATOR=:
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
            set(hpx_version 1.7.1)

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

    # if(NOT LUE_HAVE_GLFW)
    #     # Conan find module uses glfw::glfw instead of glfw
    #     add_library(glfw ALIAS glfw::glfw)
    # endif()

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
                glfw
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


if(LUE_DOCOPT_REQUIRED)
    find_package(docopt REQUIRED)

    if(NOT LUE_HAVE_DOCOPT)
        # Conan find module uses docopt::docopt instead of docopt
        add_library(docopt ALIAS docopt::docopt)
    endif()
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
endif()

if(LUE_MS_GSL_REQUIRED)
    find_package(Microsoft.GSL REQUIRED)
endif()

if(LUE_NETCDF4_REQUIRED)
    find_package(NetCDF REQUIRED)
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
