File format
===========
The LUE physical data model is a file format that is implemented on top
of HDF5. HDF5 is a data model itself, as well as a library of functions
with an associated API. More information about HDF5 can be found on the
`HDF5 support portal`_.

The LUE data model is a convention for organizing information about large
collections of objects in an HDF5 file. In HDF5 information is stored
using groups, datasets, and attributes. Groups can contain groups and
datasets. Datasets contain multidimensional arrays. Groups and dataset
can contain attributes.

In the next figure, the layout of the LUE physical data model is
shown.

.. graphviz::

    digraph physical_data_model {

        graph [
            fontname="Arial"
            fontsize=14
            margin=0.1
            nodesep=0.1
            ranksep=0.3
            bgcolor="transparent"
        ]

        node [
            fontname="Arial"
            fontsize=14
            fontcolor="white"
            shape="oval"
            style="filled"
        ]

        edge [
            fontname="Arial"
            fontsize=14
        ]

        dataset [
            label="dataset"
            fillcolor="#88C0D0"
        ]

        universes [
            label="universes"
            fillcolor="#d08770"
        ]

        universe [
            label="universe"
            fillcolor="#d08770"
        ]

        phenomena [
            label="phenomena"
            fillcolor="#b48ead"
        ]

        phenomenon [
            label="phenomenon"
            fillcolor="#b48ead"
        ]

        object_id [
            label="object-id"
            fillcolor="#434c5e"
        ]

        collection_property_sets [
            label="collection property-sets"
            fillcolor="#bf616a"
        ]

        property_sets [
            label="property-sets"
            fillcolor="#bf616a"
        ]

        property_set [
            label="property-set"
            fillcolor="#bf616a"
        ]

        object_tracker [
            label="object tracker"
            fillcolor="#434c5e"
        ]

        active_set_index [
            label="active set index"
            fillcolor="#434c5e"
        ]

        active_object_index [
            label="active object index"
            fillcolor="#434c5e"
        ]

        active_object_id [
            label="active object id"
            fillcolor="#434c5e"
        ]

        properties [
            label="properties"
            fillcolor="#a3be8c"
        ]

        property [
            label="property"
            fillcolor="#a3be8c"
        ]

        time_domain [
            label="time domain"
            fillcolor="#5e81ac"
        ]

        space_domain [
            label="space domain"
            fillcolor="#5e81ac"
        ]

        value [
            label="value"
            fillcolor="#ebcb8b"
        ]


        dataset -> universes [
            label="1"
        ]

        universes -> universe [
            label="*"
        ]

        universe -> phenomena [
            label="1"
        ]

        dataset -> phenomena [
            label="1"
        ]

        phenomena -> phenomenon [
            label="*"
        ]

        phenomenon -> collection_property_sets [
            label="1"
        ]

        collection_property_sets -> property_set [
            label="*"
        ]

        phenomenon -> property_sets [
            label="1"
        ]

        phenomenon -> object_id [
            label="1"
        ]

        property_sets -> property_set [
            label="*"
        ]

        property_set -> object_tracker [
            label="1"
        ]

        object_tracker -> active_set_index [
            label="1"
        ]

        object_tracker -> active_object_index [
            label="1"
        ]

        object_tracker -> active_object_id [
            label="1"
        ]

        property_set -> time_domain [
            label="1"
        ]

        property_set -> space_domain [
            label="1"
        ]

        property_set -> properties [
            label="1"
        ]

        properties -> property [
            label="*"
        ]

        property -> value [
            label="1"
        ]

    }

The API for storing information in a LUE dataset reflects this
layout. See for example the next example which uses the LUE Python
package and in which a new dataset is created. The object returned by
:func:`lue.data_model.create_dataset` supports adding a universe to the
universes collection and a phenomenon to the phenomena collection. The
object returned by :func:`lue.data_model.Dataset.add_phenomenon`
supports adding collection property-sets and property-sets.

.. code-block:: python

   TODO

.. _HDF5 support portal: https://portal.hdfgroup.org/display/HDF5/HDF5
