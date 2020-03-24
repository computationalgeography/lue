# -*- encoding: utf8 -*-
from ..benchmark import *
from .partition_shape_experiment import *
from ..cluster import *
from ..plot import *
from ..util import *

import lue

import dateutil.relativedelta
import dateutil.parser

import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

import json
import tempfile


# TODO Add number of partitions


def benchmark_meta_to_lue_json(
        benchmark_pathname,
        lue_pathname,
        cluster,
        benchmark,
        experiment):

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
                                    "value": [experiment.program_name]
                                },
                                {
                                    "name": "system_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [cluster.name]
                                },
                                {
                                    "name": "command",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.command_pathname]
                                },
                                {
                                    "name": "kind",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.name]
                                },
                                {
                                    "name": "scenario_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [benchmark.scenario_name]
                                },
                                {
                                    "name": "description",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.description]
                                },
                                {
                                    "name": "nr_workers",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": nr_workers
                                },
                            ]
                        }
                    ]
                }
            ]
        }
    }

    # Write results
    open(lue_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4))


def benchmark_to_lue_json(
        benchmark_pathname,
        lue_json_pathname,
        epoch):

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
            "{} > {}".format(epoch, benchmark_epoch))

    # Calculate number of seconds sinds the epoch
    time_points = [
        dateutil.parser.isoparse(timing["start"])
            for timing in benchmark_json["timings"]]
    time_points = [
        epoch_offset + int((time_point - benchmark_epoch).total_seconds())
            for time_point in time_points]
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

    array_shape = list(benchmark_json["task"]["array_shape"])
    partition_shape = list(benchmark_json["task"]["partition_shape"])

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    "property_sets": [
                        {
                            "name": "measurement",
                            "description":
                                "Information per benchmark measurement",
                            "object_tracker": {
                                "active_set_index": active_set_idx,
                                "active_object_id": active_object_id
                            },
                            "time_domain": {
                                "clock": {
                                    "epoch": {
                                        "kind": "common_era",
                                        "origin": epoch.isoformat(),
                                        "calendar": "gregorian"
                                    },
                                    "unit": time_units,
                                    "tick_period_count": 1
                                },
                                "time_point": time_points
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
                                    "value": durations
                                },
                                {
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape
                                },
                                {
                                    "name": "partition_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(partition_shape)],
                                    "value": partition_shape
                                },
                            ]
                        }
                    ]
                }
            ]
        }
    }

    # Write results
    open(lue_json_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4))


def determine_epoch(
        cluster,
        benchmark,
        experiment):

    array_shapes = experiment.array.shapes
    partition_shapes = experiment.partition.shapes

    epoch = None

    for array_shape in array_shapes:
        for partition_shape in partition_shapes:

            benchmark_pathname = experiment.benchmark_result_pathname(
                cluster.name, benchmark.scenario_name, array_shape,
                "x".join([str(extent) for extent in partition_shape]),
                "json")
            assert os.path.exists(benchmark_pathname), benchmark_pathname

            benchmark_json = json.loads(open(benchmark_pathname).read())
            benchmark_start = dateutil.parser.isoparse(benchmark_json["start"])

            if epoch is None:
                epoch = benchmark_start
            else:
                epoch = epoch if epoch < benchmark_start else benchmark_start

    return epoch


def import_raw_results(
        cluster,
        benchmark,
        experiment):
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
    epoch = determine_epoch(cluster, benchmark, experiment)

    lue_dataset_pathname = experiment.result_pathname(
        cluster.name, benchmark.scenario_name, "data", "lue")

    lue.create_dataset(lue_dataset_pathname, experiment.description)

    array_shapes = list(experiment.array.shapes)
    partition_shapes = list(experiment.partition.shapes)
    metadata_written = False

    for array_shape in array_shapes:
        for partition_shape in partition_shapes:

            result_pathname = experiment.benchmark_result_pathname(
                cluster.name, benchmark.scenario_name, array_shape,
                "x".join([str(extent) for extent in partition_shape]),
                "json")
            assert os.path.exists(result_pathname), result_pathname

            if not metadata_written:
                with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                    benchmark_meta_to_lue_json(
                        result_pathname, lue_json_file.name, cluster,
                        benchmark, experiment)
                    import_lue_json(lue_json_file.name, lue_dataset_pathname)
                metadata_written = True

            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_to_lue_json(result_pathname, lue_json_file.name, epoch)
                import_lue_json(lue_json_file.name, lue_dataset_pathname)

    lue.assert_is_valid(lue_dataset_pathname)

    return lue_dataset_pathname


def write_scaling_results(
        lue_dataset_pathname):

    lue_dataset = lue.open_dataset(lue_dataset_pathname, "w")
    count = lue_dataset.benchmark.measurement.duration.value.shape[1]

    # Write phenomenon to dataset containing the shapes of the arrays
    # used in the experiment
    # - array
    #     - array
    #         - shape
    array_shape = np.unique(
        lue_dataset.benchmark.measurement.array_shape.value[:], axis=0)
    nr_arrays = len(array_shape)

    array_phenomenon = lue_dataset.add_phenomenon("array",
        "This phenomenon stores the array shapes for which experiments "
        "where performed.")
    array_phenomenon.object_id.expand(nr_arrays)[:] = \
        np.arange(nr_arrays, dtype=np.uint64)

    array_property_set = array_phenomenon.add_property_set("array")

    array_shape_property = array_property_set.add_property(
        "shape", np.dtype(np.uint64), (2,),
        "For each dimension the extent of the partitioned array used in the "
        "experiment.")
    array_shape_property.value.expand(nr_arrays)[:] = array_shape

    # Write phenomenon to dataset containing per partition shape the
    # durations of the count experiments performed per array
    # - partition
    #     - partition
    #         - shape
    #         - duration_0 ... duration_count-1
    #         - mean_duration_0 ... mean_duration_count-1
    #         - std_duration_0 ... std_duration_count-1
    partition_shape = \
        lue_dataset.benchmark.measurement.partition_shape.value[:]
    nr_partitions = len(partition_shape)
    assert nr_partitions % nr_arrays == 0
    nr_partitions /= nr_arrays
    partition_shape = partition_shape[0:nr_partitions]

    partition_phenomenon = lue_dataset.add_phenomenon("partition",
        "For each time the experiment was repeated, and for each array "
        "shape for which the experiment was performed, this phenomenon "
        "stores durations for various partition shapes.")
    partition_phenomenon.object_id.expand(nr_partitions)[:] = \
        np.arange(nr_partitions, dtype=np.uint64)

    partition_property_set = partition_phenomenon.add_property_set("partition")

    partition_shape_property = partition_property_set.add_property(
        "shape", np.dtype(np.uint64), (2,),
        "For each dimension the extent of the partition of the partitioned "
        "array used in the experiment.")
    partition_shape_property.value.expand(nr_partitions)[:] = partition_shape

    duration = lue_dataset.benchmark.measurement.duration.value[:]
    nr_durations = len(duration)
    assert nr_durations == nr_arrays * nr_partitions

    for i in range(nr_arrays):
        duration_property = partition_property_set.add_property(
            "duration_{}".format(i), np.dtype(np.uint64), (count,),
            "For an array and for each partition, the duration(s) it "
            "took to perform count experiments.")
        duration_property.value.expand(nr_partitions)[:] = \
            duration[i * nr_partitions:(i + 1) * nr_partitions]

    # Add statistics
    if count > 1:
        mean_duration = np.mean(duration, axis=1)
        std_duration = np.std(duration, axis=1)
        assert len(mean_duration) == len(std_duration) == nr_durations

        for i in range(nr_arrays):
            mean_duration_property = partition_property_set.add_property(
                "mean_duration_{}".format(i), np.dtype(np.float64),
                "For an array and for each partition, the mean duration "
                "the {} experiments took.".format(count))
            mean_duration_property.value.expand(nr_partitions)[:] = \
                mean_duration[i * nr_partitions:(i + 1) * nr_partitions]

            std_duration_property = partition_property_set.add_property(
                "std_duration_{}".format(i), np.dtype(np.float64),
                "For an array and for each partition, the standard "
                "deviation of the durations the {} experiments took.".format(
                    count))
            std_duration_property.value.expand(nr_partitions)[:] = \
                std_duration[i * nr_partitions:(i + 1) * nr_partitions]


def post_process_raw_results(
        lue_dataset_pathname,
        plot_pathname,
        experiment):
    """
    Create plots and tables from raw benchmark results
    """
    lue_dataset = lue.open_dataset(lue_dataset_pathname)

    lue_meta_information = lue_dataset.benchmark.meta_information
    name = lue_meta_information.name.value[:][0]
    system_name = lue_meta_information.system_name.value[:][0]
    scenario_name = lue_meta_information.scenario_name.value[:][0]

    count = lue_dataset.benchmark.measurement.duration.value.shape[1]

    # The time point at which the experiment was performed is the epoch
    # of the time domain used to store the durations
    lue_clock = lue_dataset.benchmark.measurement.time_domain.clock
    assert lue_clock.nr_units == 1
    time_point_units = lue_clock.unit

    lue_epoch = lue_clock.epoch
    assert lue_epoch.kind == lue.Epoch.Kind.common_era
    assert lue_epoch.calendar == lue.Calendar.gregorian
    time_point = dateutil.parser.isoparse(lue_epoch.origin)

    # String containing time point in local time zone and conventions
    # time_point = time_point.astimezone(tzlocal.get_localzone()).strftime("%c")
    time_point = time_point.strftime("%c")

    array_shapes = lue_dataset.array.array.shape.value[:]
    assert len(array_shapes) == len(array_shapes)
    assert(np.all(array_shapes == array_shapes))

    partition_shapes = lue_dataset.partition.partition.shape.value[:]
    partition_sizes = np.prod(partition_shapes, axis=1)
    assert len(partition_shapes) == len(partition_sizes)
    nr_partitions = len(partition_shapes)


    def plot_duration(
            axis,
            array_idx,
            partition_sizes,
            partition_shapes):

        if count == 1:
            duration = lue_dataset.partition.partition \
                .properties["duration_{}".format(array_idx)].value[:]
            y_label = u"duration ({})".format(time_point_units)
            axis.plot(
                partition_sizes, duration, linewidth=default_linewidth,
                color=actual_color, marker="o")
        else:
            duration = lue_dataset.partition.partition \
                .properties["mean_duration_{}".format(array_idx)].value[:]
            error = lue_dataset.partition.partition \
                .properties["std_duration_{}".format(array_idx)].value[:]
            y_label= u"duration ({}) ± stddev (count={})".format(
                time_point_units, count)
            axis.errorbar(
                x=partition_sizes, y=duration, yerr=error,
                linewidth=default_linewidth,
                color=actual_color, marker="o")

        axis.set_xlabel(u"partition shape")
        axis.set_xticks(partition_sizes)
        axis.set_xticklabels(
            ["{}".format("{}x{} ({size})".format(
                *partition_shapes[i], size=partition_sizes[i]))
                    for i in range(len(partition_shapes))],
            rotation=45)

        axis.set_ylabel(y_label)

        axis.grid()


    # --------------------------------------------------------------------------
    # For each array shape (size) a line plot with, for a given array
    # size, the spread of durations per partition size
    # for array_shape in array_shapes:
    for array_idx in range(len(array_shapes)):

        array_shape = array_shapes[array_idx]

        figure, axis = plt.subplots(
                figsize=(15, 10)
            )  # Inches...

        plot_duration(axis, array_idx, partition_sizes, partition_shapes)

        figure.suptitle(
            "{}, {}, {}\n"
            "Partition shape scaling experiment on {} array"
                .format(
                    name,
                    system_name,
                    time_point,
                    "x".join([str(extent) for extent in array_shape]),
                )
            )

        a_plot_pathname = experiment.benchmark_result_pathname(
            system_name, scenario_name, array_shape, "plot", "pdf")
        plt.savefig(a_plot_pathname)


    # --------------------------------------------------------------------------
    array_shapes = lue_dataset.array.array.shape.value[:]
    array_sizes = np.prod(array_shapes, axis=1)
    assert len(array_shapes) == len(array_sizes)
    nr_arrays = len(array_shapes)
    nr_plots = nr_arrays

    figure, axes = plt.subplots(
            figsize=(15, nr_plots * 5), nrows=nr_plots, ncols=1,
            sharex=True, sharey=True
        )

    for array_idx in range(nr_arrays):
        axis = axes[array_idx]
        plot_duration(axis, array_idx, partition_sizes, partition_shapes)
        axis.set_title("{}x{} ({size})".format(
            *array_shapes[array_idx], size=array_sizes[array_idx]))

    figure.suptitle(
        "{}, {}, {}\n"
        "Partition shape scaling experiment"
            .format(
                name,
                system_name,
                time_point,
            )
        )

    plt.savefig(plot_pathname)


def post_process_results(
        cluster_settings_json,
        benchmark_settings_json,
        experiment_settings_json,
        command_pathname):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function.
    """
    cluster = Cluster(cluster_settings_json)
    benchmark = Benchmark(benchmark_settings_json, cluster)
    experiment = PartitionShapeExperiment(
        experiment_settings_json, command_pathname)

    lue_dataset_pathname = import_raw_results(cluster, benchmark, experiment)
    write_scaling_results(lue_dataset_pathname)

    create_dot_graph(
        experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "data", "lue"),
        experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "graph", "pdf"))
    plot_pathname = experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "plot", "pdf")
    post_process_raw_results(lue_dataset_pathname, plot_pathname, experiment)
