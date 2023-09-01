echo on

mkdir build
if errorlevel 1 exit 1

:: https://blog.knatten.org/2022/08/26/microsoft-c-versions-explained/

:: TODO Call write_conan_profile.py

:: Select the first line, remove everything before and after the version number, remove all dots,
:: grab the first three bytes
:: Example input: Microsoft (R) C/C++ Optimizing Compiler Version 19.35.32217.1 for x64
:: Results in: 193
:: See Conan's settings.yml for supported values
:: TODO How to do all this from a Windows batch file?
:: compiler_version=$($CXX 2>&1 | head -n 1 | sed 's/^.*Version // ; s/ .*// ; s/[.]//g' | head -c 3)
:: set compiler_version=193
::
:: echo [settings] ^
::
:: arch=x86_%ARCH% ^
::
:: build_type=Release ^
::
:: compiler=msvc ^
::
:: compiler.version=%compiler_version% ^
::
:: compiler.runtime=dynamic ^
::
:: compiler.cppstd=17 ^
::
:: os=Windows > host_profile
::
:: echo [settings] ^
::
:: arch=x86_%ARCH% ^
::
:: build_type=Release ^
::
:: compiler=msvc ^
::
:: compiler.version=%compiler_version% ^
::
:: compiler.runtime=dynamic ^
::
:: compiler.cppstd=17 ^
::
:: os=Windows > build_profile

%PYTHON% environment/script/write_conan_profile.py matrix.compiler.name host_profile
%PYTHON% environment/script/write_conan_profile.py matrix.compiler.name build_profile

set LUE_CONAN_PACKAGES=imgui span-lite

conan install . ^
    --profile:host=host_profile ^
    --profile:build=build_profile ^
    --build=missing ^
    --output-folder=build
if errorlevel 1 exit 1

:: Unset CMAKE_GENERATOR_PLATFORM. The platform is already specified in Conan's preset. Otherwise:
:: Generator
::     Ninja
:: does not support platform specification, but platform
::     x64
:: was specified.
set CMAKE_GENERATOR_PLATFORM=
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
