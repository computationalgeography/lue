#!/usr/bin/env bash
set -e
set -x

echo $PREFIX
echo $SP_DIR


# We need to create an out of source build
cd $SRC_DIR

mkdir -p build && cd build

PATH=$PREFIX/bin:$PATH cmake $SRC_DIR -G"Ninja" \
-D CMAKE_BUILD_TYPE=Release \
-D CMAKE_PREFIX_PATH:PATH="${PREFIX}" \
-D CMAKE_INSTALL_PREFIX:PATH="${PREFIX}" \
-D CMAKE_INSTALL_LIBDIR=lib \
-D LUE_INSTALL_PYTHON_PACKAGE_DIR="${SP_DIR}/lue" \
-D Python3_EXECUTABLE="${PYTHON}" \
-D LUE_HAVE_BOOST:BOOL=TRUE \
-D LUE_HAVE_FMT:BOOL=TRUE \
-D LUE_HAVE_PYBIND11:BOOL=TRUE \
-D LUE_HAVE_HDF5:BOOL=TRUE \
-D LUE_HAVE_GDAL:BOOL=TRUE \
-D LUE_HAVE_DOCOPT:BOOL=TRUE \
-D LUE_HAVE_GLEW:BOOL=TRUE \
-D LUE_HAVE_GLFW:BOOL=TRUE \
-D LUE_HAVE_MS_GSL:BOOL=FALSE \
-D LUE_HAVE_NLOHMANN_JSON:BOOL=TRUE \
-D LUE_DATA_MODEL_WITH_PYTHON_API=ON \
-D LUE_DATA_MODEL_WITH_UTILITIES=ON \
-D LUE_BUILD_VIEW=ON \
-D LUE_BUILD_TEST=ON \
-D LUE_BUILD_FRAMEWORK=ON \
-D LUE_FRAMEWORK_WITH_PYTHON_API=ON \
-D LUE_BUILD_HPX=ON \
-D HPX_USE_CMAKE_CXX_STANDARD=ON \
-D HPX_WITH_MALLOC="tcmalloc" \
-D HPX_WITH_PKGCONFIG=OFF \
-D HPX_WITH_EXAMPLES=OFF \
-D HPX_WITH_TESTS=OFF

# Use parallel build but not for lue.framework
cmake --build . --target lue_view lue_translate lue_validate core
cmake --build . --target all --parallel 1

ctest --extra-verbose --output-on-failure

cmake --install . --component hpx_runtime
cmake --install . --component lue_runtime
