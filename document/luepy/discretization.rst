Discretization
==============
LUE property values can be discretized. This is one way to store the continuous variation of some property value through the time and/or space domain. For example, to store the continuous variation of topographic elevation in an area, to folowing approach can be used (assuming elevation does not change over the time considered):

- property set
    - time domain: omnipresent
    - space domain: located in space, store the rectangular extent of zero or more areas
    - property
        - Named discretization, for example
        - A 1D array with 2 values per item
        - Each of the arrays contains the number of rows and columns per area
        - The property has an attribute called `lue_discretization_type` with a value of `lue_cartesian_grid`.
    - property
        - Named elevation, for example
        - A 2D array per item
        - Each of these arrays contains the elevation per cell
        - Link the elevation property to the discretization property. Name the link `lue_space_discretization`.

The result of this layout is a representation in the LUE data model of zero or more rasters. In general, the information required to understand the way a property value is discretized is stored itself in a property. This discretization property is a property like any other property, but it is linked to a property whose values are discretized using a special keyword (`lue_space_discretization` in the above example). This means that the domain of a discretization property can be different from the property being discretized.

In the folowing sections the approach is described of storing continuous varying property values according to different kinds of discretization.


Variation through time
----------------------
Regular grid
~~~~~~~~~~~~



Variation through space
-----------------------
Cartesian grid
~~~~~~~~~~~~~~
A Cartesian grid is a regular grid where the sides of the cells have equal size. For each item, the number of cells in each dimension must be stored. So, in the 2D space case, 2 values must be stored representing the number of cells in the two space dimensions. The order of these values must match the order of the dimensions in the 2D array stored in the discretized property value.


Regular grid
~~~~~~~~~~~~


Variation through time and space
--------------------------------







