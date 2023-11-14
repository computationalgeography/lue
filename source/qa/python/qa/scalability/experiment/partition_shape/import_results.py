import json
import os.path
import tempfile

import dateutil.parser
import numpy as np

import lue.data_model as ldm
from lue.qa.scalability.core import process

from .. import dataset, job
from .configuration import Configuration
from .experiment import Experiment


def benchmark_meta_to_lue_json(
    benchmark_pathname, lue_dataset_pathname, cluster, benchmark, experiment
):
    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())
    environment_json = benchmark_json["environment"]
    nr_workers = [environment_json["nr_workers"]]

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
                                    "value": [experiment.program_name],
                                },
                                {
                                    "name": "system_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [cluster.name],
                                },
                                {
                                    "name": "command",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.command_pathname],
                                },
                                {
                                    "name": "kind",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.name],
                                },
                                {
                                    "name": "scenario_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [benchmark.scenario_name],
                                },
                                {
                                    "name": "description",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.description],
                                },
                                {
                                    "name": "nr_workers",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": nr_workers,
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    # Write results
    open(lue_dataset_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4)
    )


def benchmark_to_lue_json(benchmark_pathname, lue_json_pathname, epoch):
    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())

    time_units = benchmark_json["unit"]
    benchmark_epoch = dateutil.parser.isoparse(benchmark_json["start"])

    # We assume here that benchmarks are located at different time points
    # in seconds from each other. If this is not the case, use time_units
    # to figure out what units to use instead.
    epoch_offset = int((benchmark_epoch - epoch).total_seconds())

    if epoch_offset < 0:
        raise RuntimeError(
            "epoch passed in is later than epoch from benchmark: "
            "{} > {}".format(epoch, benchmark_epoch)
        )

    # Benchmarks are sorted by benchmark epochs. Keep the information
    # sorted by time as well. Use benchmark epoch instead of individual
    # timings.
    time_points = [epoch_offset]

    property_description = "Amount of time a measurement took"
    durations = [timing["duration"] for timing in benchmark_json["timings"]]

    # shape_in_partitions = [result["shape_in_partitions"] \
    #     for result in benchmark_json["results"]]
    # nr_partitions = [
    #     float(np.prod(shape_in_partitions[i]))
    #         for i in range(len(shape_in_partitions))]

    # Object tracking: a benchmark contains property values (durations)
    # for a single object (piece of software being benchmarked). The ID of
    # this object is some value, like 5.
    # The active set indices are 0, 1, 2, 3, ...
    nr_active_sets = len(time_points)
    active_set_idx = list(range(nr_active_sets))
    active_object_id = nr_active_sets * [5]

    array_shape = list(benchmark_json["task"]["array_shape"])
    rank = len(array_shape)
    partition_shape = list(benchmark_json["task"]["partition_shape"])

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    "property_sets": [
                        {
                            "name": "measurement",
                            "description": "Information per benchmark measurement",
                            "object_tracker": {
                                "active_set_index": active_set_idx,
                                "active_object_id": active_object_id,
                            },
                            "time_domain": {
                                "clock": {
                                    "epoch": {
                                        "kind": "common_era",
                                        "origin": epoch.isoformat(),
                                        "calendar": "gregorian",
                                    },
                                    "unit": time_units,
                                    "tick_period_count": 1,
                                },
                                "time_point": time_points,
                            },
                            "properties": [
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
                                {
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape,
                                },
                                {
                                    "name": "partition_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(partition_shape)],
                                    "value": partition_shape,
                                },
                                # {
                                #     "name": "nr_partitions",
                                #     "shape_per_object": "same_shape",
                                #     "value_variability": "variable",
                                #     "shape_variability": "constant_shape",
                                #     "datatype": "uint64",
                                #     "shape": [len(nr_partitions)],
                                #     "value": nr_partitions
                                # },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    # Write results
    open(lue_json_pathname, "w").write(json.dumps(lue_json, sort_keys=False, indent=4))


def determine_epoch(result_prefix, cluster, benchmark, experiment):
    array_shapes = experiment.array.shapes
    partition_shapes = experiment.partition.shapes

    epoch = None

    for array_shape in array_shapes:
        for partition_shape in partition_shapes:
            benchmark_pathname = experiment.benchmark_result_pathname(
                result_prefix,
                cluster.name,
                benchmark.scenario_name,
                array_shape,
                "x".join([str(extent) for extent in partition_shape]),
                "json",
            )
            assert os.path.exists(benchmark_pathname), benchmark_pathname

            benchmark_json = json.loads(open(benchmark_pathname).read())
            benchmark_start = dateutil.parser.isoparse(benchmark_json["start"])

            if epoch is None:
                epoch = benchmark_start
            else:
                epoch = epoch if epoch < benchmark_start else benchmark_start

    return epoch


def import_raw_results(
    lue_dataset_pathname, result_prefix, cluster, benchmark, experiment
):
    """
    Import all raw benchmark results into a new LUE file

    This is a two step process:
    1. Translate each raw benchmark result into a LUE JSON file
    2. Import all LUE JSON files into a single LUE file
    """
    # Each benchmark containing timings has a start location in time and
    # an overall duration. The location in time can be used to position
    # the benchmark in time. Most likely, all benchmarks are started at
    # different locations in time. The duration is not that relevant.

    # The timings are represented by a location in time and a
    # duration. The location in time is not that relevant. Duration is.

    # To position all benchmarks in time, we need a single starting time
    # point to use as the clock's epoch and calculate the distance of
    # each benchmark's start time point from this epoch.
    epoch = determine_epoch(result_prefix, cluster, benchmark, experiment)

    array_shapes = list(experiment.array.shapes)
    partition_shapes = list(experiment.partition.shapes)
    metadata_written = False

    for array_shape in array_shapes:
        for partition_shape in partition_shapes:
            result_pathname = experiment.benchmark_result_pathname(
                result_prefix,
                cluster.name,
                benchmark.scenario_name,
                array_shape,
                "x".join([str(extent) for extent in partition_shape]),
                "json",
            )
            assert os.path.exists(result_pathname), result_pathname

            if not metadata_written:
                with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                    benchmark_meta_to_lue_json(
                        result_pathname,
                        lue_json_file.name,
                        cluster,
                        benchmark,
                        experiment,
                    )
                    process.import_lue_json(lue_json_file.name, lue_dataset_pathname)
                metadata_written = True

            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_to_lue_json(result_pathname, lue_json_file.name, epoch)
                process.import_lue_json(lue_json_file.name, lue_dataset_pathname)

    ldm.assert_is_valid(lue_dataset_pathname)


def write_scalability_results(lue_dataset):
    count = lue_dataset.benchmark.measurement.duration.value.shape[1]

    # Write phenomenon to dataset containing the shapes of the arrays
    # used in the experiment
    # - array
    #     - array
    #         - shape
    array_shape = np.unique(
        lue_dataset.benchmark.measurement.array_shape.value[:], axis=0
    )
    nr_arrays = len(array_shape)

    array_phenomenon = lue_dataset.add_phenomenon(
        "array",
        "This phenomenon stores the array shapes for which experiments "
        "where performed.",
    )
    array_phenomenon.object_id.expand(nr_arrays)[:] = np.arange(
        nr_arrays, dtype=np.uint64
    )

    array_property_set = array_phenomenon.add_property_set("array")

    array_shape_property = array_property_set.add_property(
        "shape",
        np.dtype(np.uint64),
        (2,),
        "For each dimension the extent of the partitioned array used in the "
        "experiment.",
    )
    array_shape_property.value.expand(nr_arrays)[:] = array_shape

    # Write phenomenon to dataset containing per partition shape the
    # durations of the count experiments performed per array
    # - partition
    #     - partition
    #         - shape
    #         - duration_0 ... duration_count-1
    #         - mean_duration_0 ... mean_duration_count-1
    #         - std_duration_0 ... std_duration_count-1
    partition_shape = lue_dataset.benchmark.measurement.partition_shape.value[:]
    nr_partitions = len(partition_shape)
    assert nr_partitions % nr_arrays == 0
    nr_partitions = nr_partitions // nr_arrays
    partition_shape = partition_shape[0:nr_partitions]

    partition_phenomenon = lue_dataset.add_phenomenon(
        "partition",
        "For each time the experiment was repeated, and for each array "
        "shape for which the experiment was performed, this phenomenon "
        "stores durations for various partition shapes.",
    )
    partition_phenomenon.object_id.expand(nr_partitions)[:] = np.arange(
        nr_partitions, dtype=np.uint64
    )

    partition_property_set = partition_phenomenon.add_property_set("partition")

    partition_shape_property = partition_property_set.add_property(
        "shape",
        np.dtype(np.uint64),
        (2,),
        "For each dimension the extent of the partition of the partitioned "
        "array used in the experiment.",
    )
    partition_shape_property.value.expand(nr_partitions)[:] = partition_shape

    duration = lue_dataset.benchmark.measurement.duration.value[:]
    nr_durations = len(duration)
    assert nr_durations == nr_arrays * nr_partitions

    for i in range(nr_arrays):
        duration_property = partition_property_set.add_property(
            "duration_{}".format(i),
            np.dtype(np.uint64),
            (count,),
            "For an array and for each partition, the duration(s) it "
            "took to perform count experiments.",
        )
        duration_property.value.expand(nr_partitions)[:] = duration[
            i * nr_partitions : (i + 1) * nr_partitions
        ]

    # Add statistics
    if count > 1:
        mean_duration = np.mean(duration, axis=1)
        std_duration = np.std(duration, axis=1)
        assert len(mean_duration) == len(std_duration) == nr_durations

        for i in range(nr_arrays):
            mean_duration_property = partition_property_set.add_property(
                "mean_duration_{}".format(i),
                np.dtype(np.float64),
                "For an array and for each partition, the mean duration "
                "the {} experiments took.".format(count),
            )
            mean_duration_property.value.expand(nr_partitions)[:] = mean_duration[
                i * nr_partitions : (i + 1) * nr_partitions
            ]

            std_duration_property = partition_property_set.add_property(
                "std_duration_{}".format(i),
                np.dtype(np.float64),
                "For an array and for each partition, the standard "
                "deviation of the durations the {} experiments took.".format(count),
            )
            std_duration_property.value.expand(nr_partitions)[:] = std_duration[
                i * nr_partitions : (i + 1) * nr_partitions
            ]


def import_results(configuration_data):
    configuration = Configuration(configuration_data)
    cluster = configuration.cluster
    benchmark = configuration.benchmark
    result_prefix = configuration.result_prefix
    experiment = configuration.experiment

    lue_dataset = job.open_raw_lue_dataset(
        result_prefix, cluster, benchmark, experiment, "r"
    )
    raw_results_already_imported = dataset.raw_results_already_imported(lue_dataset)

    cluster, benchmark, experiment = dataset.read_benchmark_settings(
        lue_dataset, Experiment
    )

    if not raw_results_already_imported:
        lue_dataset_pathname = lue_dataset.pathname
        del lue_dataset
        import_raw_results(
            lue_dataset_pathname, result_prefix, cluster, benchmark, experiment
        )

    if not raw_results_already_imported or not job.scalability_lue_dataset_exists(
        result_prefix, cluster, benchmark, experiment
    ):
        # Copy dataset and write scalability results
        job.copy_raw_to_scalability_lue_dataset(
            result_prefix, cluster, benchmark, experiment
        )
        lue_dataset = job.open_scalability_lue_dataset(
            result_prefix, cluster, benchmark, experiment, "w"
        )
        write_scalability_results(lue_dataset)

        ldm.assert_is_valid(lue_dataset, fail_on_warning=False)
