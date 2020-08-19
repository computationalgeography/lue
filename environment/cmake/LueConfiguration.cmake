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


# Options related to external software used by the project
option(LUE_BUILD_HPX
    "If HPX is required, build it instead of relying on the environment"
    FALSE)
option(LUE_BUILD_OTF2
    "If OTF2 is required, build it instead of relying on the environment"
    FALSE)


# Options tweaking the behaviour of the software
option(LUE_ASSERT_CONDITIONS
    "Assert conditions placed in the source code"
    FALSE)
option(LUE_VALIDATE_IDXS
    "Validate array indices are within array bounds (expensive!)"
    FALSE)


# Options related to the availability of external packages
if(WIN32)
    set(LUE_HAVE_BOOST_INIT FALSE)
    set(LUE_HAVE_DOXYGEN_INIT FALSE)
    set(LUE_HAVE_GDAL_INIT FALSE)
    set(LUE_HAVE_HDF5_INIT FALSE)
else()
    set(LUE_HAVE_BOOST_INIT TRUE)
    set(LUE_HAVE_DOXYGEN_INIT TRUE)
    set(LUE_HAVE_GDAL_INIT TRUE)
    set(LUE_HAVE_HDF5_INIT TRUE)
endif()

function(lue_have_option name)
    option(LUE_HAVE_${name}
        "If ${name} is required, assume it is installed"
        ${LUE_HAVE_${name}_INIT})
endfunction()

lue_have_option(BOOST)
lue_have_option(DOXYGEN)
lue_have_option(GDAL)
lue_have_option(HDF5)


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

    set(HDF5_PREFER_PARALLEL TRUE)

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
    set(LUE_IMGUI_REQUIRED TRUE)
    set(LUE_NLOHMANN_JSON_REQUIRED TRUE)
endif()


if(LUE_BUILD_DOCUMENTATION)
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

    find_package(LATEX)

    if(NOT LATEX_FOUND)
        message(WARNING
            "LaTeX could not be found. Latex documents will not be generated")
    else()
        include(UseLATEX)
    endif()
endif()


# Build (not find) external packages -------------------------------------------
# It is important to build external packages first, because our CMake
# logic might influence them. For example, HPX tests whether
# Boost.filesystem has been found already. If we do that here, before
# building HPX, the HPX build fails.
# Should be fixed in HPX>=1.5.0
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
        if(LUE_REPOSITORY_CACHE AND EXISTS ${LUE_REPOSITORY_CACHE}/hpx)
            set(hpx_repository ${LUE_REPOSITORY_CACHE}/hpx)
        else()
            set(hpx_repository https://github.com/STEllAR-GROUP/hpx)
        endif()

        if(LUE_HPX_GIT_TAG)
            set(hpx_git_tag ${LUE_HPX_GIT_TAG})
        else()
            set(hpx_git_tag "1.5.0-rc2")
        endif()

        FetchContent_Declare(hpx
            GIT_REPOSITORY ${hpx_repository}
            GIT_TAG ${hpx_git_tag}
        )

        FetchContent_MakeAvailable(hpx)
    endif()
endif()


if(LUE_IMGUI_REQUIRED AND LUE_BUILD_IMGUI)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(SDL2 REQUIRED)

    if(LUE_REPOSITORY_CACHE AND EXISTS ${LUE_REPOSITORY_CACHE}/imgui)
        set(imgui_repository ${LUE_REPOSITORY_CACHE}/imgui)
    else()
        set(imgui_repository https://github.com/ocornut/imgui.git)
    endif()

    FetchContent_Declare(imgui
        // MIT License, see ${imgui_SOURCE_DIR}/LICENSE.txt
        GIT_REPOSITORY ${imgui_repository}
        GIT_TAG v1.76
    )

    FetchContent_GetProperties(imgui)

    if(NOT imgui_POPULATED)
        FetchContent_Populate(imgui)

        add_library(imgui STATIC
            # imgui release
            ${imgui_SOURCE_DIR}/imgui
            ${imgui_SOURCE_DIR}/imgui_demo
            ${imgui_SOURCE_DIR}/imgui_draw
            ${imgui_SOURCE_DIR}/imgui_widgets

            # opengl3 / sdl2 binding
            ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl3
            ${imgui_SOURCE_DIR}/examples/imgui_impl_sdl
        )

        target_include_directories(imgui SYSTEM
            PRIVATE
                ${imgui_SOURCE_DIR}
            PUBLIC
                ${imgui_SOURCE_DIR}/examples
                $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
                ${SDL2_INCLUDE_DIR}
        )

        target_compile_options(imgui
            PUBLIC
                # Output of `sdl2-config --cflags`
                "$<$<PLATFORM_ID:Linux>:-D_REENTRANT>"
        )

        target_link_libraries(imgui
            PUBLIC
                ${SDL2_LIBRARY}
                GLEW::glew
                OpenGL::GL
        )

        add_library(imgui::imgui ALIAS imgui)
    endif()
endif()


# Find or install external packages --------------------------------------------
if(LUE_BOOST_REQUIRED)
    if(NOT LUE_HAVE_BOOST)
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} boost/1.73.0)
        set(LUE_CONAN_OPTIONS ${LUE_CONAN_OPTIONS} boost:shared=True)
    endif()
endif()

if(LUE_DOCOPT_REQUIRED)
    set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} docopt.cpp/0.6.3)
endif()

if(LUE_FMT_REQUIRED)
    set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} fmt/7.0.3)
endif()

if(LUE_GDAL_REQUIRED)
    if(NOT LUE_HAVE_GDAL)
        # https://conan.io/center/gdal/3.1.0
        set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} gdal/3.1.0)
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
        set(LUE_CONAN_OPTIONS ${LUE_CONAN_OPTIONS}
                hdf5:shared=True
                hdf5:enable_cxx=False
                hdf5:hl=False
                hdf5:threadsafe=True
                hdf5:with_zlib=True)
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
    set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} nlohmann_json/3.9.1)
endif()

if(LUE_PYBIND11_REQUIRED)
    set(LUE_CONAN_REQUIRES ${LUE_CONAN_REQUIRES} pybind11/2.5.0)
endif()


include(Conan)
run_conan()


if(LUE_BOOST_REQUIRED)
    find_package(Boost REQUIRED COMPONENTS ${LUE_REQUIRED_BOOST_COMPONENTS})

    add_definitions(
            -DBOOST_ALL_NO_LIB
            -DBOOST_ALL_DYN_LINK
        )
endif()

if(LUE_DOXYGEN_REQUIRED)
    find_package(Doxygen REQUIRED dot)
endif()

if(LUE_GDAL_REQUIRED)
    if(LUE_HAVE_GDAL)
        find_package(GDAL REQUIRED)
        set(lue_gdal_target GDAL::GDAL)
    else()
        set(lue_gdal_target CONAN_PKG::gdal)
    endif()
endif()

if(LUE_GRAPHVIZ_REQUIRED)
    find_package(Graphviz REQUIRED)

    if(GRAPHVIZ_FOUND)
        include(GraphvizMacro)
    endif()
endif()

if(LUE_HDF5_REQUIRED)
    if(LUE_HAVE_HDF5)
        find_package(HDF5 REQUIRED COMPONENTS C)
    endif()
endif()

if(LUE_OPENCL_REQUIRED)
    find_package(OpenCL REQUIRED)
endif()

if(LUE_PYBIND11_REQUIRED)
    include(${CONAN_BUILD_DIRS_PYBIND11}/pybind11Tools.cmake)
    set(LUE_PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})

    # Given Python found, figure out where the NumPy headers are. We don't
    # want to pick up headers from another prefix than the prefix of the
    # Python interpreter.
    execute_process(COMMAND "${LUE_PYTHON_EXECUTABLE}" -c
        "import numpy as np; print(\"{};{}\".format(np.__version__, np.get_include()));"
        RESULT_VARIABLE numpy_search_result
        OUTPUT_VARIABLE numpy_search_output
        ERROR_VARIABLE numpy_search_error
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT numpy_search_result MATCHES 0)
        message(FATAL_ERROR
            "${LUE_PYTHON_EXECUTABLE} is unable to import numpy:\n${numpy_search_error}")
    else()
        list(GET numpy_search_output -2 numpy_version)
        list(GET numpy_search_output -1 NUMPY_INCLUDE_DIRS)

        message(STATUS
            "Found NumPy ${numpy_version} headers in ${NUMPY_INCLUDE_DIRS}")
    endif()

    if(NOT LUE_PYTHON_API_INSTALL_DIR)
        # Most Python packages install in a subdirectory of Python's site
        # packages. But we currently ship only Python packages implemented
        # as shared libraries. Therefore, we install in the root of the
        # site packages directory. We may have to change things in
        # the future if this is unconventional.
        set(LUE_PYTHON_API_INSTALL_DIR "${PYTHON_SITE_PACKAGES}")  # /lue")
    endif()

    ### find_package(Python COMPONENTS Interpreter Development NumPy)

    ### if(NOT Python_FOUND)
    ###     message(FATAL_ERROR "Python not found")
    ### endif()

    ### if(NOT Python_NumPy_FOUND)
    ###     message(FATAL_ERROR "Python NumPy not found")
    ### endif()

    ### if(NOT LUE_PYTHON_API_INSTALL_DIR)
    ###     # Most Python packages install in a subdirectory of Python's site
    ###     # packages. But we currently ship only Python packages implemented
    ###     # as shared libraries. Therefore, we install in the root of the
    ###     # site packages directory. We may have to change things in
    ###     # the future if this is unconventional.
    ###     set(LUE_PYTHON_API_INSTALL_DIR "${Python_SITELIB}")  # /lue")
    ### endif()
endif()

if(LUE_PYTHON_REQUIRED AND NOT LUE_PYBIND11_REQUIRED)
    find_package(Python COMPONENTS Interpreter)

    if(NOT Python_FOUND)
        message(FATAL_ERROR "Python not found")
    endif()

    set(LUE_PYTHON_EXECUTABLE ${Python_EXECUTABLE})
endif()

if(LUE_SPHINX_REQUIRED)
    # TODO Find Sphinx Python package.
    include(SphinxDoc)

    if(NOT SPHINX_BUILD_EXECUTABLE OR NOT SPHINX_APIDOC_EXECUTABLE)
        message(FATAL_ERROR "sphinx not found")
    endif()
endif()
