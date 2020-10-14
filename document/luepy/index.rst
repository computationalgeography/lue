LUE Python package
==================
The LUE Python package consists of two submodules: :mod:`lue.data_model` and :mod:`lue.framework`.

The functionality in :mod:`lue.data_model` allows you to perform I/O to the `LUE Scientific Database`, which is an `HDF5`_ binary data format. The data model lets you efficiently store huge amounts of attribute values of huge amounts of objects. These objects can be located in time and/or in space, and the attribute values can be n-dimensional values.

The functionality in :mod:`lue.framework` allows you to create scalable environmental models that can be run on laptops, desktop and clusters of nodes.

The LUE data model and framework are `developed <https://github.com/computationalgeography/lue>`_ by the `Computational Geography`_ R&D Team, to be useful in the domain of agent-based and field-based modeling. The data format is general enough to allow it to be used in other domains as well.


.. note::

    The LUE Python package is currently in development and not ready
    to be used in general. Unless you are involved in the project we
    suggest you come back here once in a while to check on the project's
    status.


.. note::

    LUE is pronounced as the French pronounce the name Louis (`LU-EE`).


.. toctree::
   :maxdepth: 2

   introduction
   installation
   quick_start
   user_manual
   Case studies <case_study/index>
   changes


.. ifconfig:: apidoc_generated

.. toctree::
   :maxdepth: 2

   API <api>


.. _Computational Geography: https://www.computationalgeography.org
.. _HDF5: https://www.hdfgroup.org/HDF5/


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

