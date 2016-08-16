#!/usr/bin/env bash
set -e

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
cd $tmp_prefix


# Install prerequisites.
cd $local_prefix
wget https://github.com/pybind/pybind11/archive/v1.8.1.tar.gz
tar zxf v1.8.1.tar.gz
cpath=$local_prefix/pybind11-1.8.1/include
cd ..


# Build LUE.
cd $build_prefix
cmake -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_INSTALL_PREFIX=$install_prefix -DLUE_BUILD_PYTHON_API:BOOL=TRUE -DCMAKE_PREFIX_PATH=$cpath $source_prefix


# Install files needed for creating the LUE Python package.
cmake --build . --target lue_python_package_sources


# Show what we've got.
cd $install_prefix
tree .


# Create source distribution.
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
