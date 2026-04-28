#!/usr/bin/env python
import sys
from pathlib import Path

import docopt
import numpy as np

import lue.framework as lfr
from lue import __version__ as lue_version


# from lue.qa.scalability import perform_experiment_task
# from lue.qa.scalability.core import json_to_data


@lfr.runtime_scope
def create_raster(arguments) -> int:
    # TODO This function returns None in case of error. Fix this (in decorator?).
    array_shape = tuple(int(extent) for extent in arguments["<shape>"].split(","))
    string_to_type = {
        "uint8": np.uint8,
        "uint32": np.uint32,
        "uint64": np.uint64,
        "int32": np.int32,
        "int64": np.int64,
        "float32": np.float32,
        "float64": np.float64,
    }
    element_type = string_to_type[arguments["<type>"]]

    def value_or_none(name):
        return (
            element_type(arguments[f"<{name}>"])
            if arguments[f"<{name}>"] is not None
            else None
        )

    fill_value = value_or_none("fill")
    min_value = value_or_none("min")
    max_value = value_or_none("max")
    raster_path = Path(arguments["<name>"])

    status = 0

    try:
        if fill_value is not None:
            raster = lfr.create_array(
                array_shape=array_shape, dtype=element_type, fill_value=fill_value
            )
        else:
            raster = lfr.uniform(
                array_shape=array_shape,
                dtype=element_type,
                min_value=min_value,
                max_value=max_value,
            )
        lfr.to_gdal(raster, str(raster_path))
    except Exception as exception:
        sys.stderr.write(f"{exception}\n")
        status = 1

    return status


def main() -> int:
    command = Path(sys.argv[0]).name
    usage = f"""\
Create raster with dummy data

Usage:
    {command} <shape> <type> <fill> <name>
    {command} <shape> <type> <min> <max> <name>

Arguments:
    shape       Array shape. Format rows,cols
    type        Element type. Valid types are: uint8, uint32, uint64, int32,
                int64, float32, float64
    fill        Fill value
    min         Minimum element value
    max         Maximum element value
    name        Pathname of raster

Options:
    -h --help   Show this screen and exit
    --version   Show version and exit
"""
    argv = [arg for arg in sys.argv[1:] if not arg.startswith("--hpx:")]
    arguments = docopt.docopt(usage, argv, version=lue_version)

    return create_raster(arguments)


if __name__ == "__main__":
    sys.exit(main())
