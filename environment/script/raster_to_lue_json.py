#!/usr/bin/env python
import json
import os.path
import sys

import docopt


usage = """\
Create a LUE JSON file representing a collection of rasters

Usage:
    {command} [--prefix=<prefix>]
        <west> <north> <south> <east>
        <nr_rows> <nr_cols>
        <name>...

Options:
    prefix      Pathname to directory containing rasters
    name        Names of rasters to represent
    -h --help   Show this screen

It is assumed that all rasters occupie the same area in space and have the
same resolution. If necessary, this information will be read from one
of the rasters, which therefore must exist.

The prefix directory is used to make relative paths to rasters
absolute. Absolute paths to rasters will be used as is.
""".format(
    command=os.path.basename(sys.argv[0])
)


def raster_to_lue_json(raster_pathnames, lue_file, extent, discretization):

    object_id = 5
    properties = [
        {
            "name": os.path.splitext(os.path.basename(pathname))[0],
            "value_variability": "constant",
            "shape_per_object": "different_shape",
            "datatype": "float32",
            "rank": 2,
            "value": [{"id": object_id, "dataset": pathname}],
            "space_discretization": {
                "type": "regular_grid",
                "property": "/areas/globals/discretization",
            },
        }
        for pathname in raster_pathnames
    ]

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "areas",
                    "object_id": [object_id],
                    "collection_property_sets": [
                        {
                            "name": "globals",
                            "properties": [
                                {
                                    "name": "discretization",
                                    "value_variability": "constant",
                                    "shape_per_object": "same_shape",
                                    "datatype": "uint64",
                                    "shape": [2],
                                    "value": discretization,
                                }
                            ],
                        }
                    ],
                    "property_sets": [
                        {
                            "name": "rasters",
                            "space_domain": {
                                "rank": 2,
                                "datatype": "float64",
                                "space_box": extent,
                            },
                            "properties": properties,
                        }
                    ],
                }
            ]
        }
    }

    lue_file.write(json.dumps(lue_json, sort_keys=False, indent=4))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    prefix = arguments["--prefix"]
    names = arguments["<name>"]
    raster_pathnames = []

    for name in names:
        if (not os.path.isabs(name)) and prefix:
            raster_pathnames.append(os.path.normpath(os.path.join(prefix, name)))
        else:
            raster_pathnames.append(os.path.abspath(name))

    west = float(arguments["<west>"])
    north = float(arguments["<north>"])
    south = float(arguments["<south>"])
    east = float(arguments["<east>"])
    nr_rows = int(arguments["<nr_rows>"])
    nr_cols = int(arguments["<nr_cols>"])

    lue_file = sys.stdout

    raster_to_lue_json(
        raster_pathnames, lue_file, [west, north, south, east], [nr_rows, nr_cols]
    )
