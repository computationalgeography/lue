[TOC]

# lue_hdf5  {#lue_hdf5}


# Introduction  {#lue_hdf5_introduction}
The `lue_hdf5` library wraps the HDF5 C-API using C++. The goal is to make
the HDF5 API more convenient to use. The library is used by the LUE
C++ library, but it might also be useful in other projects, not related
to LUE. See @ref lue_hdf5_example for an example CMake project using
only the `lue_hdf5` library.


# Example  {#lue_hdf5_example}
Lets assume we need to write an empty HDF5 file called `hello_world.h5`:

~~~cpp
// hello_hdf5.cpp
#include <lue/hdf5.hpp>


void main()
{
    lue::hdf5::create_file("hello_world.h5");
}
~~~


First we need to build and install the `lue_hdf5` library to some location
(assuming that the LUE sources are in `/tmp/lue_source`):

~~~bash
mkdir -p /tmp/lue_build
cd /tmp/lue_build
cmake \
    -DLUE_BUILD_CXX_API=FALSE\
    -DLUE_BUILD_PYTHON_API=FALSE
    -DLUE_BUILD_UTILITIES=FALSE
    -DCMAKE_INSTALL_PREFIX=/tmp/lue_install
    /tmp/lue_source
cmake --build . --target install
~~~


Now we can write a CMake project file for building our project:

~~~cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.9)
project(hello_hdf5 LANGUAGES CXX)

find_package(lue)

add_executable(hello_hdf5
    hello_hdf5)

target_link_libraries(hello_hdf5
    PRIVATE
        lue::lue_hdf5
)
~~~

In this project, we tell CMake to try and find the `lue` package. When
this succeeds, we can use the `lue::lue_hdf5` library target to link
against our application. Our application will now automatically depend
on the public dependencies of `lue_hdf5`.

To make CMake find the LUE package, we need to tell it where it is
installed. Given the example above the folowing commands build our
`hello_hdf5` project:


~~~bash
mkdir -p /tmp/hello_hdf5_build
cd /tmp/hello_hdf5lue_build
cmake \
    -DCMAKE_PREFIX_PATH="/tmp/lue_install"
    /tmp/hello_hdf5
cmake --build .
~~~

We can now run the resulting executable:
~~~bash
./hello_hdf5
~~~


# See also  {#lue_hdf5_see_also}
- [Collective alling requirements in parallel HDF5 applications](https://support.hdfgroup.org/HDF5/doc/RM/CollectiveCalls.html)

