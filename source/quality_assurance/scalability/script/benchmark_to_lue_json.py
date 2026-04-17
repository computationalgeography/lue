#!/usr/bin/env python
import json
import os.path
import sys

import dateutil.parser
import dateutil.relativedelta
import docopt


usage = """\
Translate a benchmark result formatted as JSON into a LUE JSON file

Usage:
    {command} [--epoch=<epoch>] <benchmark_file> <lue_file>
    {command} --meta <benchmark_file> <lue_file>

Options:
    benchmark_file  Pathname to existing file containing benchmark JSON
    lue_file        Pathname to new file for storing LUE JSON
    --epoch=<epoch>  Replace epoch with this one
    -h --help       Show this screen

The resulting file can be imported into a LUE dataset using the
lue_translate command. With lue_translate multiple JSON files can be
imported. In that case it can be useful to use the --epoch option to
synchronize the epochs from the different JSON files.

Use the --meta option to only write the meta information to a LUE
JSON file. This information does not change between benchmark runs,
so it should be generated and translated only once.
""".format(command=os.path.basename(sys.argv[0]))


def benchmark_meta_to_lue_json(benchmark_pathname, lue_pathname):
    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    "collection_property_sets": [
                        {
                            "name": "meta_information",
                            "properties": [
                                {
                                    "name": "name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(benchmark_json["name"])],
                                },
                                {
                                    "name": "system_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [
                                        "{}".format(benchmark_json["system_name"])
                                    ],
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    # Write results
    open(lue_pathname, "w").write(json.dumps(lue_json, sort_keys=False, indent=4))


def benchmark_to_lue_json(benchmark_pathname, lue_pathname, epoch=None):
    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())

    time_units = "second"
    benchmark_epoch = dateutil.parser.parse(benchmark_json["start"])
    epoch = benchmark_epoch if epoch is None else epoch
    epoch_offset = int((benchmark_epoch - epoch).total_seconds())

    if epoch_offset < 0:
        raise RuntimeError(
            "epoch passed in is later than epoch from benchmark: {} > {}".format(
                epoch, benchmark_epoch
            )
        )

    # Calculate number of seconds sinds the epoch
    time_points = [
        dateutil.parser.parse(timing["start"]) for timing in benchmark_json["timings"]
    ]
    time_points = [
        epoch_offset + int((time_point - benchmark_epoch).total_seconds())
        for time_point in time_points
    ]
    time_points = [time_points[0]]

    property_description = "Amount of time a measurement took"
    durations = [timing["duration"] for timing in benchmark_json["timings"]]

    # Object tracking: a benchmark contains property values (durations)
    # for a single object (piece of software being benchmarked). The ID of
    # this object is some value, like 5.
    # The active set indices are 0, 1, 2, 3, ...
    nr_active_sets = len(time_points)
    active_set_idx = list(range(nr_active_sets))
    active_object_id = nr_active_sets * [5]

    environment_json = benchmark_json["environment"]
    nr_localities = [environment_json["nr_localities"]]
    nr_threads = [environment_json["nr_threads"]]
    work_size = [environment_json["work_size"]]

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    # "collection_property_sets": [
                    #     {
                    #         "name": "meta_information",
                    #         "properties": [
                    #             {
                    #                 "name": "name",
                    #                 "shape_per_object": "same_shape",
                    #                 "value_variability": "constant",
                    #                 "datatype": "string",
                    #                 "value": ["{}".format(
                    #                     benchmark_json["name"])]
                    #             },
                    #         ]
                    #     }
                    # ],
                    "property_sets": [
                        {
                            "name": "measurement",
                            "description": "Information per benchmark measurement",
                            "object_tracker": {
                                "active_set_index": active_set_idx,
                                "active_object_id": active_object_id,
                            },
                            "time_domain": {
                                # "item_type": "time_point",
                                "clock": {
                                    "unit": time_units,
                                    "tick_period_count": 1,
                                    "epoch": epoch.isoformat(),
                                },
                                "time_point": time_points,
                            },
                            "properties": [
                                {
                                    "name": "nr_localities",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "value": nr_localities,
                                },
                                {
                                    "name": "nr_threads",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "value": nr_threads,
                                },
                                {
                                    "name": "work_size",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "value": work_size,
                                },
                                {
                                    "name": "duration",
                                    "description": property_description,
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(durations)],
                                    "value": durations,
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    # Write results
    open(lue_pathname, "w").write(json.dumps(lue_json, sort_keys=False, indent=4))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    benchmark_pathname = arguments["<benchmark_file>"]
    lue_pathname = arguments["<lue_file>"]

    if arguments["--meta"]:
        benchmark_meta_to_lue_json(benchmark_pathname, lue_pathname)
    else:
        epoch = arguments["--epoch"]
        epoch = dateutil.parser.parse(epoch) if epoch else epoch
        benchmark_to_lue_json(benchmark_pathname, lue_pathname, epoch=epoch)
