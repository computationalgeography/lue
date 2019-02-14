# LUE  {#mainpage}

[TOC]


The LUE software contains two main parts: code related to the physical
data model and code related to the framework for environmental modelling.


# Data model  {#lue_data_model}
The data model code is divided into libraries that depend on each
other. From lower to higher levels of abstraction these are:

- lue_hdf5: C++ wrappers around the HDF5 C-library
- lue_cxx: Implementation and API of the data model code
- lue_python: Python API of the data model code

Besides the libraries the data model code contains the following command
line utilities:

- lue_translate: For translating between LUE and other data formats
- lue_validate: For validating LUE datasets
- lue_view: For viewing the contents of LUE datasets


# Framework  {#lue_framework}



