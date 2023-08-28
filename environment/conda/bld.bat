mkdir build
if errorlevel 1 exit 1


echo [settings] ^

arch=x86_%ARCH% ^

build_type=Release ^

compiler=msvc ^

compiler.cppstd=17 ^

compiler.version=%VS_MAJOR% ^

os=Windows > host_profile

echo [settings] ^

arch=x86_%ARCH% ^

build_type=Release ^

compiler=msvc ^

compiler.cppstd=17 ^

compiler.version=%VS_MAJOR% ^

os=Windows > build_profile


set LUE_CONAN_PACKAGES="imgui span-lite"

conan install . ^
    --profile:build=build_profile ^
    --profile:host=host_profile ^
    --build=missing ^
    --output-folder=build

set CMAKE_PREFIX_PATH=build

cmake --preset conan-release ^
    -D CMAKE_INSTALL_PREFIX:PATH="%LIBRARY_PREFIX%" ^
    -D CMAKE_INSTALL_LIBDIR=lib ^
    -D LUE_INSTALL_PYTHON_PACKAGE_DIR="%SP_DIR%/lue" ^
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE ^
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE ^
    -D LUE_BUILD_VIEW=TRUE ^
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE ^
    -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE ^
    -D Python3_EXECUTABLE="%PYTHON%"
if errorlevel 1 exit 1

cmake --build build --target all
if errorlevel 1 exit 1

cmake --install build --component lue_runtime
if errorlevel 1 exit 1
