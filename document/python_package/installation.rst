Installation
============

.. highlight:: bash

Source installation on Linux
----------------------------
The LUE package can be installed with `pip`_::

    $ pip install --pre lue

The ``--pre`` argument is required because only pre-release versions of LUE are available at the moment. You can pass an explicit version number by replacing the ``lue`` package name argument by ``lue==<version>``. To upgrade an existing LUE installation you must add the ``--upgrade`` argument to the ``install`` command::

    $ pip install --pre --upgrade lue

During the installation a C++ compiler will be used to compiler some source files. The LUE C++ sources depend on the folowing packages to be installed:

- `pybind11`_ C++ library (header-only)
- `hdf5`_ C library (library and headers)

In case installation fails because pybind11's header files cannot be found, you can pass the location of these files explicitly on the command line::

    $ CPATH=<path_to_pybind11> pip install --pre lue


.. _pip: https://pip.pypa.io/en/stable/
.. _pybind11: https://github.com/pybind/pybind11
.. _hdf5: https://www.hdfgroup.org/HDF5/


Testing the installation
------------------------
After LUE has been installed you can test whether all went well using this command::

    $ python -c "import lue; print(lue.__version__)"

It should print the version number of the LUE package you installed.
