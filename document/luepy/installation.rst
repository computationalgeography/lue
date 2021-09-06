Installation
============

.. highlight:: bash

Source installation on Linux
----------------------------
The LUE package can be installed with `conda`::

    $ conda install lue

..
    You can pass an explicit version number by replacing the ``lue`` package name argument by ``lue==<version>``. To upgrade an existing LUE installation you must add the ``--upgrade`` argument to the ``install`` command::

        $ pip install --upgrade lue

    During the installation a C++ compiler will be used to compile some source files. The LUE C++ sources depend on the folowing packages to be installed:

    - `boost`_ C++ libraries (headers)
    - `hdf5`_ C library (library and headers)
    - `numpy`_ Python extension (headers)
    - `pybind11`_ C++ library (header-only)

    In case installation fails because header and/or library files cannot be found, you can pass the location of these files explicitly on the command line::

        $ pip install \
            --global-option="-I/some_inc_dir:/some_other_inc_dir" \
            --global-option="-L/some_lib_dir:/some_other_lib_dir" \
            lue


    .. _pip: https://pip.pypa.io/en/stable/
    .. _pybind11: https://github.com/pybind/pybind11
    .. _hdf5: https://www.hdfgroup.org/HDF5/
    .. _boost: http://www.boost.org
    .. _numpy: http://www.numpy.org


Testing the installation
------------------------
After LUE has been installed you can test whether all went well using this command::

    $ python -c "import lue; print(lue.__version__)"

It should print the version number of the LUE package you installed.
