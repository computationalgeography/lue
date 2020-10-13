.. _install_source:

Install from source
===================
The LUE source code can be obtained from the `LUE Github project
page`_. CMake is used to configure and generate project files for
building LUE.

..
   TODO General, use of CMake, configure, build install

The following CMake variables are used to determine which targets to build:

================================ ======= ===========================
Variable                         Default Effect
================================ ======= ===========================
`LUE_BUILD_DATA_MODEL`           `TRUE`  Build data model C++ API
`LUE_BUILD_FRAMEWORK`            `FALSE` Build modelling framework
`LUE_BUILD_VIEW`                 `FALSE` Build data model viewer
`LUE_BUILD_DOCUMENTATION`        `FALSE` Build API documentation
`LUE_BUILD_TEST`                 `FALSE` Build test targets
================================ ======= ===========================

Here is an example session of building the default LUE targets and
installing them in `$HOME/lue_install`:

.. code-block:: bash

   cd /tmp
   git clone https://github.com/pcraster/lue.git
   cd lue
   mkdir build
   cd build
   cmake -DCMAKE_INSTALL_PREFIX=$HOME/lue_install ..
   cmake --build . --target install

Also, you can check the scripts behind the `LUE Github workflows`_
to see how LUE can be built on various platforms.

If one of the Python API's is built, the installation directory can
be set manually by setting `LUE_PYTHON_API_INSTALL_DIR`. By default,
it will be set to the pathname of the current Python's site packages.


Targets
+++++++

Data model
----------
Optional features:

================================ ======= ===========================
Variable                         Default Effect
================================ ======= ===========================
`LUE_DATA_MODEL_WITH_PYTHON_API` `FALSE` Build data model Python API
`LUE_DATA_MODEL_WITH_UTILITIES`  `FALSE` Build commandline utilities
================================ ======= ===========================


Dependencies
~~~~~~~~~~~~
- HDF5 C, Boost (filesystem, system), fmt
- `LUE_DATA_MODEL_WITH_PYTHON_API`: Python, pybind11, NumPy, Guidelines Support libraries
- `LUE_DATA_MODEL_WITH_UTILITIES`: Docopt, GDAL, Nlohmann JSON


Framework
---------
Optional features:

================================ ======= ====================================
Variable                         Default Effect
================================ ======= ====================================
`LUE_FRAMEWORK_WITH_PYTHON_API`  `FALSE` Build modelling framework Python API
================================ ======= ====================================


Dependencies
~~~~~~~~~~~~
- Boost, Docopt, fmt, HPX, Kokkos mdspan
- `LUE_FRAMEWORK_WITH_PYTHON_API`: Python, pybind11, NumPy


View
----


Dependencies
~~~~~~~~~~~~
- Dear ImGUI, OpenGL, GLEW, SDL2


Documentation
-------------


Dependencies
~~~~~~~~~~~~
- Breathe, Doxygen, Graphviz, Sphinx


Test
----


Dependencies
~~~~~~~~~~~~
- Boost unit-test framework library


Dependencies
++++++++++++
There are three options for installing prerequisite libraries and tools:

1. Install using platform specific package manager
2. Install using Conan, either before building LUE or by the LUE build
   scripts
3. Build by the LUE build scripts

These variables determine what should happen:

- `LUE_HAVE_BOOST`
    - TRUE: Search for Boost in standard locations
    - FALSE: Install Boost using Conan
    - Default: WIN32: FALSE, All other platforms: TRUE
- `LUE_HAVE_GDAL`
    - TRUE: Search for GDAL in standard locations
    - FALSE: Install GDAL using Conan
    - Default: WIN32: FALSE, All other platforms: TRUE
- `LUE_HAVE_HDF5`
    - TRUE: Search for HDF5 in standard locations
    - FALSE: Install HDF5 using Conan
    - Default: WIN32: FALSE, All other platforms: TRUE

Some prerequisite libraries can optionally be built by the LUE project
itself. The following variables are used to decide whether or not that
is the case. By default, they are not set.

================ ===================================================
Variable         Effect when set
================ ===================================================
`LUE_BUILD_HPX`  Build the HPX library
`LUE_BUILD_OTF2` Build the OTF2 library (optionally required by HPX)
================ ===================================================

.. _LUE Github project page: https://github.com/pcraster/lue
.. _LUE Github workflows: https://github.com/pcraster/lue/actions
