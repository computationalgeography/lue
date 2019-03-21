#!/usr/bin/env python
import os.path
import sys
sys.path = [
    os.path.join(os.path.split(__file__)[0], "..", "..", "benchmark")
] + sys.path
import lue.benchmark
import docopt
import json


usage = """\
Generate job scheduler script for benchmarking

Usage:
    {command} partition_shape <cluster_settings> <benchmark_settings> <experiment_settings> <script> <command>
    {command} strong_scaling <cluster_settings> <benchmark_settings> <experiment_settings> <script> <command>
    {command} weak_scaling <cluster_settings> <benchmark_settings> <experiment_settings> <script> <command>

Options:
    partition_shape   Benchmark for partition shape
    strong_scaling    Benchmark for strong scaling characteristics
    weak_scaling      Benchmark for weak scaling characteristics
    cluster_settings  Pathname of JSON file containing cluster settings
    experiment_settings  Pathname of JSON file containing experiment settings
    script            Pathname of shell script to generate
    command           Command to benchmark
    -h --help         Show this screen

The generated shell script will submit all required tasks to the scheduler

Experiment settings:

    partition_shape
        ...

    strong_scaling
        ...

    weak_scaling
        ...
""".format(
    command = os.path.basename(sys.argv[0]))


### def benchmark_meta_to_lue_json(
###         benchmark_pathname,
###         lue_pathname):
### 
###     # Read benchmark JSON
###     benchmark_json = json.loads(open(benchmark_pathname).read())
### 
###     lue_json = {
###         "dataset": {
###             "phenomena": [
###                 {
###                     "name": "benchmark",
###                     "collection_property_sets": [
###                         {
###                             "name": "meta_information",
###                             "properties": [
###                                 {
###                                     "name": "name",
###                                     "shape_per_object": "same_shape",
###                                     "value_variability": "constant",
###                                     "datatype": "string",
###                                     "value": ["{}".format(
###                                         benchmark_json["name"])]
###                                 },
###                                 {
###                                     "name": "system_name",
###                                     "shape_per_object": "same_shape",
###                                     "value_variability": "constant",
###                                     "datatype": "string",
###                                     "value": ["{}".format(
###                                         benchmark_json["system_name"])]
###                                 },
###                             ]
###                         }
###                     ]
###                 }
###             ]
###         }
###     }
### 
###     # Write results
###     open(lue_pathname, "w").write(
###         json.dumps(lue_json, sort_keys=False, indent=4))
### 
### 
### def benchmark_to_lue_json(
###         benchmark_pathname,
###         lue_pathname,
###         epoch=None):
### 
###     # Read benchmark JSON
###     benchmark_json = json.loads(open(benchmark_pathname).read())
### 
###     time_units = "second"
###     benchmark_epoch = dateutil.parser.parse(benchmark_json["start"])
###     epoch = benchmark_epoch if epoch is None else epoch
###     epoch_offset = int((benchmark_epoch - epoch).total_seconds())
### 
###     if epoch_offset < 0:
###         raise RuntimeError(
###             "epoch passed in is later than epoch from benchmark: "
###             "{} > {}".format(epoch, benchmark_epoch))
### 
###     # Calculate number of seconds sinds the epoch
###     time_points = [
###         dateutil.parser.parse(timing["start"])
###             for timing in benchmark_json["timings"]]
###     time_points = [
###         epoch_offset + int((time_point - benchmark_epoch).total_seconds())
###             for time_point in time_points]
###     time_points = [time_points[0]]
### 
###     property_description = "Amount of time a measurement took"
###     durations = [timing["duration"] for timing in benchmark_json["timings"]]
### 
###     # Object tracking: a benchmark contains property values (durations)
###     # for a single object (piece of software being benchmarked). The ID of
###     # this object is some value, like 5.
###     # The active set indices are 0, 1, 2, 3, ...
###     nr_active_sets = len(time_points)
###     active_set_idx = list(range(nr_active_sets))
###     active_object_id = nr_active_sets * [5]
### 
###     environment_json = benchmark_json["environment"]
###     nr_localities = [environment_json["nr_localities"]]
###     nr_threads = [environment_json["nr_threads"]]
###     work_size = [environment_json["work_size"]]
### 
###     lue_json = {
###         "dataset": {
###             "phenomena": [
###                 {
###                     "name": "benchmark",
###                     # "collection_property_sets": [
###                     #     {
###                     #         "name": "meta_information",
###                     #         "properties": [
###                     #             {
###                     #                 "name": "name",
###                     #                 "shape_per_object": "same_shape",
###                     #                 "value_variability": "constant",
###                     #                 "datatype": "string",
###                     #                 "value": ["{}".format(
###                     #                     benchmark_json["name"])]
###                     #             },
###                     #         ]
###                     #     }
###                     # ],
###                     "property_sets": [
###                         {
###                             "name": "measurement",
###                             "description":
###                                 "Information per benchmark measurement",
###                             "object_tracker": {
###                                 "active_set_index": active_set_idx,
###                                 "active_object_id": active_object_id
###                             },
###                             "time_domain": {
###                                 # "item_type": "time_point",
###                                 "clock": {
###                                     "unit": time_units,
###                                     "tick_period_count": 1,
###                                     "epoch": epoch.isoformat()
###                                 },
###                                 "time_point": time_points
###                             },
###                             "properties": [
###                                 {
###                                     "name": "nr_localities",
###                                     "shape_per_object": "same_shape",
###                                     "value_variability": "variable",
###                                     "shape_variability": "constant_shape",
###                                     "datatype": "uint64",
###                                     "value": nr_localities
###                                 },
###                                 {
###                                     "name": "nr_threads",
###                                     "shape_per_object": "same_shape",
###                                     "value_variability": "variable",
###                                     "shape_variability": "constant_shape",
###                                     "datatype": "uint64",
###                                     "value": nr_threads
###                                 },
###                                 {
###                                     "name": "work_size",
###                                     "shape_per_object": "same_shape",
###                                     "value_variability": "variable",
###                                     "shape_variability": "constant_shape",
###                                     "datatype": "uint64",
###                                     "value": work_size
###                                 },
###                                 {
###                                     "name": "duration",
###                                     "description": property_description,
###                                     "shape_per_object": "same_shape",
###                                     "value_variability": "variable",
###                                     "shape_variability": "constant_shape",
###                                     "datatype": "uint64",
###                                     "shape": [len(durations)],
###                                     "value": durations
###                                 }
###                             ]
###                         }
###                     ]
###                 }
###             ]
###         }
###     }
### 
###     # Write results
###     open(lue_pathname, "w").write(
###         json.dumps(lue_json, sort_keys=False, indent=4))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    ### benchmark_pathname = arguments["<benchmark_file>"]
    ### lue_pathname = arguments["<lue_file>"]

    ### if arguments["--meta"]:
    ###     benchmark_meta_to_lue_json(benchmark_pathname, lue_pathname)
    ### else:
    ###     epoch = arguments["--epoch"]
    ###     epoch = dateutil.parser.parse(epoch) if epoch else epoch
    ###     benchmark_to_lue_json(benchmark_pathname, lue_pathname, epoch=epoch)

    cluster_settings_pathname = arguments["<cluster_settings>"]
    cluster_settings = json.load(open(cluster_settings_pathname))

    benchmark_settings_pathname = arguments["<benchmark_settings>"]
    benchmark_settings = json.load(open(benchmark_settings_pathname))

    experiment_settings_pathname = arguments["<experiment_settings>"]
    experiment_settings = json.load(open(experiment_settings_pathname))

    script_pathname = arguments["<script>"]
    command_pathname = arguments["<command>"]

    if arguments["partition_shape"]:
        lue.benchmark.partition_shape(
            cluster_settings, benchmark_settings, experiment_settings,
            script_pathname, command_pathname)
    elif arguments["strong_scaling"]:
        lue.benchmark.strong_scaling()
    elif arguments["weak_scaling"]:
        lue.benchmark.weak_scaling()
