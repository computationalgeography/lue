mkdir build
cd build
if errorlevel 1 exit 1

REM TODO Use Ninja here
cmake .. ^
    -G "NMake Makefiles" ^
    -D CMAKE_BUILD_TYPE=Release ^
    -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
    -D CMAKE_INSTALL_LIBDIR=lib ^
    -D LUE_INSTALL_PYTHON_PACKAGE_DIR="%SP_DIR%/lue" ^
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE ^
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE ^
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE ^
    -D LUE_BUILD_QA=TRUE ^
    -D LUE_QA_WITH_PYTHON_API=TRUE ^
    -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE ^
    -D Python3_EXECUTABLE="%PYTHON%"
if errorlevel 1 exit 1

REM TODO Remove parallel once Ninja is used
cmake --build . --config Release --parallel %CPU_COUNT%
if errorlevel 1 exit 1

cmake --install . --component lue_runtime
if errorlevel 1 exit 1
