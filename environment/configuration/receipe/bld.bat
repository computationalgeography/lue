@echo on

rem We need to create an out of source build

md %TMPDIR%\build

if %errorlevel% neq 0 exit /b %errorlevel%

cd %TMPDIR%\build

if %errorlevel% neq 0 exit /b %errorlevel%

rem Ensure desired Boost version is selected by CMake
set "BOOST_ROOT=%PREFIX%"
set "BOOST_NO_SYSTEM_PATHS=ON"

cmake %SRC_DIR% ^
    -G "Visual Studio 16 2019" -A x64 ^
    -D FETCHCONTENT_QUIET=FALSE ^
    -D CMAKE_BUILD_TYPE=Release ^
    -D CMAKE_PREFIX_PATH:PATH="%LIBRARY_PREFIX%" ^
    -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
    -D LUE_BUILD_DATA_MODEL=TRUE ^
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE ^
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE ^
    -D LUE_BUILD_FRAMEWORK=TRUE ^
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE ^
    -D LUE_BUILD_VIEW=TRUE ^
    -D LUE_BUILD_TEST=TRUE ^
    -D LUE_TEST_NR_LOCALITIES_PER_TEST=1 ^
    -D LUE_TEST_NR_THREADS_PER_LOCALITY=2 ^
    -D LUE_BUILD_QA=TRUE ^
    -D LUE_QA_WITH_PYTHON_API=TRUE ^
    -D LUE_BUILD_HPX=TRUE ^
    -D HPX_USE_CMAKE_CXX_STANDARD=TRUE ^
    -D HPX_WITH_MALLOC="system" ^
    -D HPX_WITH_FETCH_ASIO=TRUE ^
    -D HPX_WITH_PKGCONFIG=FALSE ^
    -D HPX_WITH_EXAMPLES=FALSE ^
    -D HPX_WITH_TESTS=FALSE ^
    -D LUE_HAVE_BOOST=TRUE ^
    -D LUE_HAVE_DOCOPT=FALSE ^
    -D LUE_HAVE_FMT=FALSE ^
    -D LUE_HAVE_GDAL=TRUE ^
    -D LUE_HAVE_GLFW=FALSE ^
    -D LUE_HAVE_HDF5=TRUE ^
    -D LUE_HAVE_NLOHMANN_JSON=FALSE ^
    -D LUE_HAVE_PYBIND11=FALSE ^
    -D Boost_USE_STATIC_LIBS=FALSE ^
    -D HDF5_USE_STATIC_LIBRARIES=FALSE ^
    -D HWLOC_ROOT=%CONDA_PREFIX%/Library ^
    -D HWLOC_LIBRARY=%CONDA_PREFIX%/Library/hwloc.dll.lib ^
    -D Python3_EXECUTABLE="%PYTHON%"


if %errorlevel% neq 0 exit /b %errorlevel%

rem Use parallel build for as many targets as possible, but not for framework/algorithm
cmake --build . --config Release --target all_build

if %errorlevel% neq 0 exit /b %errorlevel%

ctest --extra-verbose --output-on-failure --build-config Release

if %errorlevel% neq 0 exit /b %errorlevel%

cmake --install . --component core
cmake --install . --component parallelism
cmake --install . --component runtime
cmake --install . --component lue_runtime

if %errorlevel% neq 0 exit /b %errorlevel%
