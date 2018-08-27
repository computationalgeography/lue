# LUE
LUE Scientific Database


More information about LUE:

|                  |                                      |
| -----------------|--------------------------------------|
| Documentation    | http://lue.readthedocs.io/en/latest/ |
| Python package   | https://pypi.python.org/pypi/lue     |
| Development team | http://www.pcraster.eu               |



[![Linux build Status](https://travis-ci.org/pcraster/lue.svg?branch=master)](https://travis-ci.org/pcraster/lue) [![Documentation Status](https://readthedocs.org/projects/docs/badge/)](https://lue.readthedocs.io/en/latest)


## Installation
Here, we describe how to configure, build and install the LUE targets using CMake. This is useful in case you want to use targets that are not installed when installing the LUE Python package (like API documentation and commandline utilities). In case you are only interested in using the LUE Python package, [installing LUE using pip](https://lue.readthedocs.io/en/latest/installation.html) is probably more convenient.

The LUE source code can be obtained from the [LUE Github project page](https://github.com/pcraster/lue). CMake is used to configure and generate project files for building LUE.

The folowing CMake variables are used to determine which targets to build:

| Variable | Default | Effect |
| -------- | ------  | ------ |
| `LUE_BUILD_ALL` | `FALSE` | Build all targets |
| `LUE_BUILD_PYTHON_API` | `FALSE` | Build Python package |
| `LUE_BUILD_UTILITIES` | `TRUE` | Build commandline utilities |
| `LUE_BUILD_CXX_API` | `TRUE` | Build C++ API |
| `LUE_BUILD_HDF5_API` | `TRUE` | Build C++ HDF5 API |
| `LUE_BUILD_DOCUMENTATION` | `FALSE` | Build C++ API documentation |
| `LUE_BUILD_TEST` | `FALSE` | Build test targets |

Setting some of these variables implies the setting of other variables:

| Variable | Implies |
| -------- | ------- |
| `LUE_BUILD_ALL` | `LUE_BUILD_PYTHON_API`, `LUE_BUILD_UTILITIES` |
| `LUE_BUILD_PYTHON_API` | `LUE_BUILD_CXX_API` |
| `LUE_BUILD_UTILITIES` | `LUE_BUILD_CXX_API` |
| `LUE_BUILD_CXX_API` | `LUE_BUILD_HDF5_API` |


Depending on which targets are selected for building, some prerequisites libaries and tools must be installed:

| Variable | Prerequisites |
| -------- | ------------- |
| `LUE_BUILD_HDF5_API` | HDF5 C, fmt libraries |
| `LUE_BUILD_CXX_API` | Boost filesystem, system libraries |
| `LUE_BUILD_UTILITIES` | Docopt, GDAL, Nlohmann JSON libraries |
| `LUE_BUILD_PYTHON_API` | Python, pybind11, NumPy, Guidelines Support libraries |
| `LUE_BUILD_TEST` | Boost unit-test framework library |
| `LUE_BUILD_DOCUMENTATION` | Doxygen, Graphviz, Sphinx |


Here is an example session of building the default LUE targets and installing them in `$HOME/lue_install`:

```bash
cd /tmp
# Recursive is used to also checkout submodules
git clone --recursive https://github.com/pcraster/lue.git
cd lue
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/lue_install ..
cmake --build . --target install
```
