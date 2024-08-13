[TOC]

# lue_translate  {#lue_translate}


# Introduction  {#lue_translate_introduction}

TODO


# Import  {#lue_translate_import}

TODO


## GDAL raster  {#lue_translate_import_gdal_raster}

TODO


## GDAL raster stack  {#lue_translate_import_gdal_raster_stack}

TODO


## GDAL feature  {#lue_translate_import_gdal_feature}

TODO


# Export  {#lue_translate_export}

## DOT  {#lue_translate_export_to_dot}

To get an impression of the layout of a LUE dataset, a
[Graphviz](http://www.graphviz.org) graph in DOT format can be generated
and written to a file. The `dot` tool can then be used to generate yet
another file, to be used for visualization. This can be a file formatted
as SVG, PDF, or PNG, for example.

~~~bash
lue_translate export --meta dot_properties.json my_dataset.lue my_dataset.dot
~~~

The conversion process can be tweaked using the metadata file. This file
can contains three sections:

| group | role                           |
| ----- | ------------------------------ |
| dot   | Configuration of the DOT graph |
| hdf5  | Formatting HDF5 objects        |
| lue   | Formatting LUE objects         |

The folowing listing shows an example.

~~~json
{
    "dot": {
        "node": {
            "fontcolor": "white"
        }
    },

    "hdf5": {
        "dataset": {
            "shape": "oval"
        },
        "dataset": {
            "shape": "box"
        }
    },

    "lue": {
        "phenomenon": {
            "fillcolor": "#791d72"
        },
        "property_set": {
            "fillcolor": "#9a0000"
        },
        "property": {
            "fillcolor": "#419702"
        },
        "domain": {
            "fillcolor": "#094d8e"
        },
        "space_domain": {
            "fillcolor": "#36a2c9"
        },
        "value": {
            "fillcolor": "#f08000",
            "show_details": false
        }
    }
}
~~~


This command can be used to convert a file containing a graph to an SVG
file for visualization:

~~~bash
dot -Tsvg -o my_dataset.svg my_dataset.dot
~~~


## Shapefile  {#lue_translate_export_to_shapefile}

A single domain in a LUE dataset can be translated to a Shapefile:

~~~bash
lue_translate export --meta meta.json my_dataset.lue my_dataset.shp
~~~

The domain-items end up as geometries in the Shapefile. Which domain
to translate is determined by the contents of the metadata file. In the
folowing example, (the domain of) `my_property_set` is selected.

~~~json
{
    "my_dataset": {
        "phenomena": [
            {
                "name": "my_phenomenon",
                "property_sets": [
                    {
                        "name": "my_property_set"
                    }
                ]
            }
        ]
    }
}
~~~
