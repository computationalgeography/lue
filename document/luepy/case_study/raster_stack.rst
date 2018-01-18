.. _raster_stack:

Raster stack
============
A raster stack is an extension of the case of a :ref:`raster <raster>`
where, besides discretizing space in raster cells, time is discretized in
time steps. This can be used to capture the magnitude of some continuous
varying field, like air pressure, both though space and time.

A stack of rasters is a discretization of a value within a time and
space extent. The next sections describe how a single temporal stack of
rasters can be captured by the LUE conceptual data model. The result is
capable of capturing many temporal stacks of rasters (for multiple items).


time domain
-----------
A stack's location in time is modelled by a 1D time box. This is an
extent, defining the part of the time domain within which the stack's
values are defined.


space domain
------------
A stack's location in space is modelled by a 2D space box. This is an
extent, defining the part of the space domain within which the stack's
values are defined.


property
--------
Three properties are needed to model the discretized value within the
stack's time and space boxes.

- A value property which contains a 3D array of cell values.
- Time and space discretization properties which contain the information
  required to interpret the 3D values stored in the value property.

The discretization properties are linked to from the value property.

.. note::

   There is a subtle difference between a time and space cube containing
   a 3D array of values and a time and space cube containing a 3D
   discretized value. In the first case, the 3D array is representative
   for the whole time and space cube, while in the latter case, each
   individual cell value in the 3D array is mapped to a cubic cell in
   the cube. Whether or not the value is discretized is what makes these
   two kinds of data different.

See the case of a :ref:`raster <raster>` for information about storing rasters
whose values don't change through time.


Example code
------------
.. literalinclude:: elevation.py
.. generated_image:: elevation.svg
