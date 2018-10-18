#!/usr/bin/env python
import dateutil.relativedelta
import dateutil.parser
import docopt
import json
import os.path
import sys


usage = """\
Translate a benchmark result formatted as JSON into a LUE JSON file

Usage:
    {command} <benchmark_file> <lue_file>

Options:
    benchmark_file  Pathname to existing file containing benchmark JSON
    lue_file        Pathname to new file for storing LUE JSON
    -h --help       Show this screen

The resulting file can be imported into a LUE dataset using the
lue_translate command.
""".format(
    command = os.path.basename(sys.argv[0]))


def benchmark_to_lue_json(
        benchmark_pathname,
        lue_pathname):

    # Read benchmark JSON
    benchmark_json = json.loads(file(benchmark_pathname).read())




    phenomenon_name = "benchmark"
    property_set_name = "measurement"
    property_set_description = "Information per benchmark measurement"

    time_units = "second"
    epoch = dateutil.parser.parse(benchmark_json["start"])

    # Calculate number of seconds sinds the epoch
    time_points = [
        dateutil.parser.parse(timing["start"])
            for timing in benchmark_json["timings"]]
    time_points = [
        int((time_point - epoch).total_seconds())
            for time_point in time_points]

    property_name = "duration"
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
    nr_threads = [environment_json["nr_threads"]]

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "environment",
                    "object_id": [0],
                    "property_sets": [
                        {
                            "name": "environment",
                            "properties": [
                                {
                                    "name": "nr_threads",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint32",
                                    "value": nr_threads
                                }
                            ]
                        }
                    ]
                },
                {
                    "name": phenomenon_name,
                    "property_sets": [
                        {
                            "name": property_set_name,
                            "description": property_set_description,
                            "object_tracker": {
                                "active_set_index": active_set_idx,
                                "active_object_id": active_object_id
                            },
                            "time_domain": {
                                # "item_type": "time_point",
                                "clock": {
                                    "unit": time_units,
                                    "tick_period_count": 1,
                                    "epoch": epoch.isoformat()
                                },
                                "time_point": time_points
                            },
                            "properties": [
                                {
                                    "name": property_name,
                                    "description": property_description,
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "value": durations
                                }
                            ]
                        }
                    ]
                }
            ]
        }
    }

    # Write results
    file(lue_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    benchmark_pathname = arguments["<benchmark_file>"]
    lue_pathname = arguments["<lue_file>"]

    benchmark_to_lue_json(benchmark_pathname, lue_pathname)
