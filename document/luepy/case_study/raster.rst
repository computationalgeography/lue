.. _raster:

Raster
======
A raster is a 2-dimensional grid of cells, where each cell represents
a square piece of the space and contains a value. Rasters are often
used to capture the magnitude of some continuous varying field, like
topographic elevation, or surface temperature.

A raster is a discretization of a value within a spatial extent. This
value does not change through time. The next sections describe how a
single raster can be captured by the LUE conceptual data model. The
result is capable of capturing multiple rasters for multiple items.


time domain
-----------
A raster's location and cell values don't change through time, so the
time domain is marked as omnipresent. See the case of a
:ref:`raster stack <raster_stack>` for information about storing rasters
whose values do change through time.


space domain
------------
A raster's location is modelled by a 2D space box. This is an extent,
defining the part of the space domain within which the raster's values
are defined.


property
--------
Two properties are needed to model the discretized value within the
raster's space box:

- A value property which contains a 2D array of cell values.
- A discretization property which contains the information required to
  interpret the 2D values stored in the value property.

The discretization property is linked to from the value property.

.. note::

    There is a somewhat subtle difference between a space box containing
    a 2D array of values and a space box containing a 2D discretized
    value. In the first case, the 2D array is representative for the
    whole space box, while in the latter case, each individual cell
    value in the 2D array is mapped to a rectangular cell in the space
    box. Whether or not the value is discretized is what makes these
    two kinds of data different.


Example code
------------
.. literalinclude:: areas.py
.. generated_image:: areas.svg
