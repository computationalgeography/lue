#!/usr/bin/env bash
set -e
set -x

# It is best to run this script with no LUE project-specific environment
# variables set. We don't want the test logic to pick up development files.


build_type="Debug"
source_prefix="$PROJECTS/lue"
tmp_prefix="$HOME/tmp/lue"
build_prefix="$tmp_prefix/build"
install_prefix="$tmp_prefix/install"
local_prefix="$tmp_prefix/local"
test_prefix="$tmp_prefix/test"


# Refresh temp location for our files.
rm -fr $tmp_prefix
mkdir -p $tmp_prefix/{build,install,local,test}


# Install prerequisites.
cd $tmp_prefix
git clone --recursive https://github.com/geoneric/peacock.git
cd $build_prefix
mkdir peacock
cd peacock
cmake -Dpeacock_prefix=$local_prefix \
    -Dbuild_boost=true -Dboost_version=1.57.0 \
        -Dboost_build_boost_test=TRUE \
    -Dbuild_docopt=true -Ddocopt_version=0.6.1 \
    -Dbuild_gdal=true -Dgdal_version=2.0.1 \
    -Dbuild_hdf5=true -Dhdf5_version=1.8.14 \
    -Dbuild_pybind11=true -Dpybind11_version=1.8.0 \
    $tmp_prefix/peacock
cmake --build . --target all


# cd $local_prefix
# wget https://github.com/pybind/pybind11/archive/v1.8.1.tar.gz
# tar zxf v1.8.1.tar.gz
# cpath=$local_prefix/pybind11-1.8.1/include
# cd ..


# Build LUE.
cd $build_prefix
cmake -DCMAKE_BUILD_TYPE=$build_type -DPEACOCK_PREFIX:PATH=$local_prefix -DCMAKE_INSTALL_PREFIX=$install_prefix -DLUE_BUILD_PYTHON_API:BOOL=TRUE $source_prefix
# -DCMAKE_PREFIX_PATH=$cpath $source_prefix
cmake --build . --target all


# Install files needed for creating the LUE Python package.
cmake --build . --target lue_python_package_sources


# Show what we've got.
cd $install_prefix
tree .


# Create source distribution.
# TODO: Add paths to peacock prefix stuff.
# CPLUS_INCLUDE_PATH="$local_prefix/*/*/*/*/include" \
export CPLUS_INCLUDE_PATH="$local_prefix/linux/linux/gcc-4/x86_64/include"
LIBRARY_PATH="$local_prefix/*/*/*/*/lib" \
    python setup.py sdist


# Install package in virtual environment and perform some tests.
virtualenv $tmp_prefix/python
source $tmp_prefix/python/bin/activate
cd $tmp_prefix
CPATH=$cpath \
    pip install --pre --no-index --find-links=file://$install_prefix/dist \
        "lue==0.0.0.dev0"

echo "********************************************************"
python -c \
    "import lue; print(\"installed LUE version {}\".format(lue.__version__))"
echo "********************************************************"


# # Create platform wheel.
# # Only supported on Windows and OSX:
# python setup.py bdist_wheel


# # Upload distributions.
# twine upload dist/*
