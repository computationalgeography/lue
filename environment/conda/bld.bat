mkdir build
if errorlevel 1 exit 1

REM TODO Figure out compiler version. Should be 19x.
REM https://blog.knatten.org/2022/08/26/microsoft-c-versions-explained/

REM Select the first line, remove everything before and after the version number, remove all dots,
REM grab the first three bytes
REM Example input: Microsoft (R) C/C++ Optimizing Compiler Version 19.35.32217.1 for x64
REM Results in: 193
REM See Conan's settings.yml for supported values
REM TODO How to do all this from a Windows batch file?
REM compiler_version=$($CXX 2>&1 | head -n 1 | sed 's/^.*Version // ; s/ .*// ; s/[.]//g' | head -c 3)

set compiler_version=193

conan profile detect

echo [settings] ^

arch=x86_%ARCH% ^

build_type=Release ^

compiler=msvc ^

compiler.version=%compiler_version% ^

compiler.runtime=dynamic ^

compiler.cppstd=17 ^

os=Windows > host_profile

echo [settings] ^

arch=x86_%ARCH% ^

build_type=Release ^

compiler=msvc ^

compiler.version=%compiler_version% ^

compiler.runtime=dynamic ^

compiler.cppstd=17 ^

os=Windows > build_profile


set LUE_CONAN_PACKAGES="imgui span-lite"

conan install . ^
    --profile:build=build_profile ^
    --profile:host=host_profile ^
    --build=missing ^
    --output-folder=build
if errorlevel 1 exit 1

set CMAKE_PREFIX_PATH=build

cmake --preset conan-release ^
    -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
    -D CMAKE_INSTALL_LIBDIR=lib ^
    -D LUE_INSTALL_PYTHON_PACKAGE_DIR="%SP_DIR%/lue" ^
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE ^
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE ^
    -D LUE_BUILD_VIEW=TRUE ^
    -D LUE_BUILD_QA=TRUE ^
    -D LUE_QA_WITH_PYTHON_API=TRUE ^
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE ^
    -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE ^
    -D Python3_EXECUTABLE="%PYTHON%"
if errorlevel 1 exit 1

cmake --build build --target all
if errorlevel 1 exit 1

cmake --install build --component lue_runtime
if errorlevel 1 exit 1
