@echo on

rem We need to create an out of source build

if %errorlevel% neq 0 exit /b %errorlevel%

md %TMPDIR%/build

dir %TMPDIR%
dir %TMPDIR%/build

if %errorlevel% neq 0 exit /b %errorlevel%

cd %TMPDIR%/build

if %errorlevel% neq 0 exit /b %errorlevel%

rem Ensure desired Boost version is selected by CMake
set "BOOST_ROOT=%PREFIX%"
set "BOOST_NO_SYSTEM_PATHS=ON"


cmake %SRC_DIR% -G"Ninja" ^
    -D CMAKE_BUILD_TYPE=Release ^
    -D CMAKE_PREFIX_PATH:PATH="%LIBRARY_PREFIX%" ^
    -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
    -D LUE_DATA_MODEL_WITH_PYTHON_API=ON ^
    -D LUE_DATA_MODEL_WITH_UTILITIES=ON ^
    -D LUE_BUILD_VIEW=ON ^
    -D Boost_USE_STATIC_LIBS=OFF ^
    -D LUE_HAVE_BOOST=TRUE ^
    -D LUE_HAVE_GDAL=TRUE ^
    -D LUE_HAVE_HDF5=TRUE ^
    -D LUE_HAVE_FMT=TRUE ^
    -D HDF5_USE_STATIC_LIBRARIES=OFF ^
    -D Python3_FIND_STRATEGY="LOCATION" ^
    -D Python3_EXECUTABLE="%PYTHON%" ^
    -D PYTHON_EXECUTABLE="%PYTHON%" ^
    -D Python_ROOT_DIR="%PREFIX%/bin" ^
    -D Python3_ROOT_DIR="%PREFIX%/bin"

if %errorlevel% neq 0 exit /b %errorlevel%

# Use parallel build for as many targets as possible, but not for framework/algorithm
cmake --build . --config Release --target source/{data_model,view}/all source/framework/{core,partitioned_array}/all

if %errorlevel% neq 0 exit /b %errorlevel%

# Build remaining targets with fewer cores. Compiling these modules requires more memory.
cmake --build . --config Release --target all --parallel 2

if %errorlevel% neq 0 exit /b %errorlevel%

ctest --extra-verbose --output-on-failure --build-config Release

if %errorlevel% neq 0 exit /b %errorlevel%

cmake --install . --component core
cmake --install . --component parallelism
cmake --install . --component runtime
cmake --install . --component lue_runtime

if %errorlevel% neq 0 exit /b %errorlevel%
