mkdir build
cd build
if %errorlevel% neq 0 exit /b %errorlevel%

cmake %SRC_DIR% ^
    -G "Visual Studio 16 2019" -A x64 ^
    -D CMAKE_BUILD_TYPE=Release ^
    -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
    -D CMAKE_INSTALL_LIBDIR=lib ^
    -D LUE_INSTALL_PYTHON_PACKAGE_DIR="%SP_DIR%/lue" ^
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE ^
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE ^
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE ^
    -D LUE_BUILD_QA=TRUE ^
    -D LUE_QA_WITH_PYTHON_API=TRUE ^
    -D LUE_HAVE_BOOST=TRUE ^
    -D LUE_HAVE_DOCOPT=TRUE ^
    -D LUE_HAVE_FMT=TRUE ^
    -D LUE_HAVE_GDAL=TRUE ^
    -D LUE_HAVE_HDF5=TRUE ^
    -D LUE_HAVE_NLOHMANN_JSON=TRUE ^
    -D LUE_HAVE_PYBIND11=TRUE ^
    -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE ^
    -D Python3_EXECUTABLE="%PYTHON%"
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --build . --config Release --target all_build
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --install . --component lue_runtime
if %errorlevel% neq 0 exit /b %errorlevel%
