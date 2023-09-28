lue_calculate
=============
The lue_calculate command has been created to make it as easy as possible to perform LUE
computations from the command-line. As an argument, the command takes a single assignment
statement of which the right-hand side defines what needs to be computed, and the left-hand
side defines the names of the output dataset(s). To clarify this, here are some examples:

.. code-block:: bash

   lue_calculate 'slope.tif = slope("$MY_DATA/elevation.tif", 100.0)'
   lue_calculate 'flux, state = accu_threshold("flow_direction", "material", "threshold")'

When all you want to do is to compute one or a few results, then lue_calculate can be of use
for you, as it is not needed to write a C++ program or Python script for that.

By default, lue_calculate will use all CPU cores available to it. Pass `HPX command-line options`_
if you want to configure the HPX runtime.

.. _HPX command-line options: https://hpx-docs.stellar-group.org/latest/html/manual/launching_and_configuring_hpx_applications.html#hpx-command-line-options

For more information, type ``lue_calculate --help`` on the command-line.
