LUE is software for storing and manipulating large amounts of information for large numbers of objects. This
information can optionally have a reference in time and space. For example, LUE can represent collections of
wandering animals and their properties, some of which are changing through time, or the elevation of the
surface of one or multiple areas. The software is useful, and currently mainly used, in the context of
environmental modelling of biological and physical systems, represented by agents and fields, but we make sure
that the software is as generic as possible, allowing it to be useful in other contexts as well.

Currently, LUE contains two main parts: *LUE data model* and *LUE framework*. LUE data model is an
implementation of the LUE physical data model, which allows users to perform I/O to the "LUE Scientific
Database". It allows for the storage of large numbers of objects and their location in time and space, and
their properties.

LUE framework is a collection of data types and algorithms that can be combined to translate large amounts of
information. It allows computations to be performed on hardware ranging from laptops to compute clusters,
without the user having to know about high-performance computing and the related technology.
