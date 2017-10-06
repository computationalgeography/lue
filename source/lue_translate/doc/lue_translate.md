[TOC]

# lue_translate  {#lue_translate}



# Introduction  {#lue_translate_introduction}
Blahblahblah


# LUE → Shapefile  {#lue_translate_lue_to_shapefile}

A single domain in a LUE dataset can be translated to a Shapefile:

~~~bash
lue_translate export --meta meta.json my_dataset.lue my_dataset.shp
~~~

The domain-items end up as geometries in the Shapefile. Which domain to translate is determined by the contents of the metadata file. In the folowing example, (the domain of) `my_property_set` is selected.

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
