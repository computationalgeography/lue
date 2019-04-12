# -*- encoding: utf8 -*-
from ..benchmark import *
from .partition_shape_experiment import *
from ..cluster import *
from ..util import *

import lue

import dateutil.relativedelta
import dateutil.parser

import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import pandas as pd
import seaborn as sns
# import tzlocal

import json
import math
import tempfile


def benchmark_meta_to_lue_json(
        benchmark_pathname,
        lue_pathname,
        cluster,
        experiment):

    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())
    environment_json = benchmark_json["environment"]
    nr_localities = [environment_json["nr_localities"]]
    nr_threads = [environment_json["nr_threads"]]

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
                                    "name": "description",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.description]
                                },
                                {
                                    "name": "nr_localities",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": nr_localities
                                },
                                {
                                    "name": "nr_threads",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": nr_threads
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
                                        "kind": "anno_domini",
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
        experiment):

    array_shapes = experiment.array.shapes()
    partition_shapes = experiment.partition.shapes()

    epoch = None

    for array_shape in array_shapes:
        for partition_shape in partition_shapes:

            benchmark_pathname = experiment.benchmark_result_pathname(
                cluster.name, array_shape,
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
    epoch = determine_epoch(cluster, experiment)

    lue_dataset_pathname = experiment.result_pathname(
        cluster.name, "data", "lue")

    if os.path.exists(lue_dataset_pathname):
        os.remove(lue_dataset_pathname)

    array_shapes = experiment.array.shapes()
    partition_shapes = experiment.partition.shapes()
    metadata_written = False

    for array_shape in array_shapes:
        for partition_shape in partition_shapes:

            result_pathname = experiment.benchmark_result_pathname(
                cluster.name, array_shape,
                "x".join([str(extent) for extent in partition_shape]),
                "json")
            assert os.path.exists(result_pathname), result_pathname

            if not metadata_written:
                with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                    benchmark_meta_to_lue_json(
                        result_pathname, lue_json_file.name, cluster, experiment)
                    import_lue_json(lue_json_file.name, lue_dataset_pathname)
                metadata_written = True

            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_to_lue_json(result_pathname, lue_json_file.name, epoch)
                import_lue_json(lue_json_file.name, lue_dataset_pathname)

    lue.assert_is_valid(lue_dataset_pathname)


def meta_information_dataframe(
        lue_meta_information):

    # TODO: BUG: double delete
    # lue_meta_information.properties["kind"].value[0:0]

    assert \
        lue_meta_information.properties["kind"].value[:] == ["partition_shape"]

    name = lue_meta_information.properties["name"].value[:]
    system_name = lue_meta_information.properties["system_name"].value[:]

    # # Pandas does not support nD array elements. Convert (each) shape
    # # to string.
    # assert array_shape.dtype == np.uint64
    # array_shape = [str(shape) for shape in array_shape]

    meta_information = pd.DataFrame({
            "name": name,
            "system_name": system_name,
        })

    return meta_information


def measurement_dataframe(
        lue_measurement):

    array_shape = lue_measurement.properties["array_shape"].value[:]
    partition_shape = lue_measurement.properties["partition_shape"].value[:]
    duration = lue_measurement.properties["duration"].value[:]
    assert len(duration) == len(partition_shape) == len(array_shape)

    # count durations per benchmark
    duration = pd.DataFrame(duration)

    # array shape per benchmark
    array_shape = pd.DataFrame(array_shape)

    # partition shape per benchmark
    partition_shape = pd.DataFrame(partition_shape)

    assert (duration.index == partition_shape.index).all()
    assert (duration.index == array_shape.index).all()

    measurement = pd.concat([array_shape, partition_shape, duration], axis=1)

    rank = len(array_shape.columns)
    count = len(duration.columns)
    labels = \
        ["array_shape_{}".format(i) for i in range(rank)] + \
        ["partition_shape_{}".format(i) for i in range(rank)] + \
        ["duration_{}".format(i) for i in range(count)]

    measurement.columns = labels

    return measurement


def post_process_raw_results(
        cluster,
        experiment):
    """
    Create plots and tables from raw benchmark results
    """
    lue_dataset_pathname = experiment.result_pathname(
        cluster.name, "data", "lue")
    lue_dataset = lue.open_dataset(lue_dataset_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]
    lue_meta_information = \
        lue_benchmark.collection_property_sets["meta_information"]
    lue_measurement = lue_benchmark.property_sets["measurement"]

    meta_information = meta_information_dataframe(lue_meta_information)
    name = meta_information.name[0]
    system_name = meta_information.system_name[0]

    rank = lue_measurement.properties["array_shape"].value.shape[1]
    count = lue_measurement.properties["duration"].value.shape[1]

    measurement = measurement_dataframe(lue_measurement)

    array_shape_labels = \
        ["array_shape_{}".format(i) for i in range(rank)]
    partition_shape_labels = \
        ["partition_shape_{}".format(i) for i in range(rank)]

    measurement.insert(0, "array_size",
        measurement.filter(items=array_shape_labels).product(axis=1))
    measurement.insert(0, "partition_size",
        measurement.filter(items=partition_shape_labels).product(axis=1))

    # The time point at which the experiment was performed is the epoch
    # of the time domain used to store the durations
    lue_clock = lue_measurement.time_domain.clock
    assert lue_clock.nr_units == 1
    time_point_units = lue_clock.unit

    lue_epoch = lue_clock.epoch
    assert lue_epoch.kind == lue.Epoch.Kind.anno_domini
    assert lue_epoch.calendar == lue.Calendar.gregorian
    time_point = dateutil.parser.isoparse(lue_epoch.origin)


    # String containing time point in local time zone and conventions
    # time_point = time_point.astimezone(tzlocal.get_localzone()).strftime("%c")
    time_point = time_point.strftime("%c")


    actual_color = sns.xkcd_rgb["denim blue"]


    # --------------------------------------------------------------------------
    # For each array shape (size) a line plot with, for a given array
    # size, the spread of durations per partition size
    for array_shape in experiment.array.shapes():

        # Given shape of array, ѕelect data to plot
        expression = " and ".join([
            "array_shape_{} == {}".format(i, array_shape[i])
                for i in range(rank)])
        measurement_per_array_shape = measurement.query(expression)

        # Select data needed for plotting
        measurement_per_array_shape = measurement_per_array_shape.filter(
            items=
                ["partition_size"] +
                ["duration_{}".format(i) for i in range(count)])

        # Durations per partition size
        measurement_per_array_shape = \
            measurement_per_array_shape.set_index(keys="partition_size")
        measurement_per_array_shape = pd.DataFrame(
            data=measurement_per_array_shape.stack(),
            columns=["duration"])

        measurement_per_array_shape.index = \
            measurement_per_array_shape.index.droplevel(1)

        # Create a new index, moving partition_size index level into columns
        measurement_per_array_shape = measurement_per_array_shape.reset_index()

        figure, axes = plt.subplots(
                nrows=1, ncols=1,
                figsize=(15, 5)
            )  # Inches...

        sns.lineplot(
            data=measurement_per_array_shape,
            x="partition_size", y="duration",
            ax=axes, color=actual_color,
            legend=False)

        # Annotation
        axes.set_ylabel(u"duration ± 1 std ({})".format(time_point_units))
        axes.yaxis.set_major_formatter(
            ticker.FuncFormatter(
                lambda y, pos: format_duration(y)))

        axes.set_xlabel(u"partition size")
        axes.xaxis.set_major_formatter(
            ticker.FuncFormatter(
                lambda x, pos: format_partition_size(x)))

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

        plot_pathname = experiment.benchmark_result_pathname(
            cluster.name, array_shape, "plot", "pdf")
        plt.savefig(plot_pathname)


    # --------------------------------------------------------------------------
    # Create a grid of line plots

    # Select data needed for plotting
    measurement = measurement.filter(
        items=
            ["array_size", "partition_size"] +
            ["duration_{}".format(i) for i in range(count)])

    # Durations per array size x partition size
    measurement = measurement.set_index(keys=["array_size", "partition_size"])
    measurement = pd.DataFrame(
        data=measurement.stack(),
        columns=["duration"])

    # Get rid of the column name (duration_i) index level
    measurement.index = measurement.index.droplevel(2)

    # Create a new index, moving array_size and partition_size index
    # levels into columns
    measurement = measurement.reset_index()

    # Now we have:
    # <index> | array_size | partition_size | duration |
    # This is 'tidy' data: each column is a variable and each row an
    # observation

    nr_plots = measurement["array_size"].nunique()
    grid = sns.FacetGrid(measurement,
        col="array_size", col_wrap=int(math.floor(math.sqrt(nr_plots))),
        height=5, aspect=3)
    grid.map(sns.lineplot, "partition_size", "duration", color=actual_color)

    # Annotation
    # TODO: Get rid of array_size = .... Replace with '... x ... array' like
    #       on the individual plots
    grid.set_axis_labels(
        u"partition size",
        u"duration ± 1 std ({})".format(time_point_units)
    )

    axes = grid.axes[0]

    axes.yaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda y, pos: format_duration(y)))

    axes.xaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda x, pos: format_partition_size(x)))

    grid.fig.suptitle(
        "{}, {}, {}\n"
        "Partition shape scaling experiment"
            .format(
                name,
                system_name,
                time_point,
            )
        )

    plot_pathname = experiment.result_pathname(cluster.name, "plot", "pdf")
    plt.savefig(plot_pathname)


    # --------------------------------------------------------------------------
    # Create surface plot
    # TODO When really needed


def post_process_results(
        cluster_settings_json,
        benchmark_settings_json,  # Not used atm...
        experiment_settings_json,
        command_pathname):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function.
    """
    job_scheduler = cluster_settings_json["job_scheduler"]
    assert job_scheduler in ["shell", "slurm"]

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)

    # benchmark = Benchmark(benchmark_settings_json)
    experiment = PartitionShapeExperiment(
        experiment_settings_json, command_pathname)

    import_raw_results(cluster, experiment)
    create_dot_graph(
        experiment.result_pathname(cluster.name, "data", "lue"),
        experiment.result_pathname(cluster.name, "graph", "pdf"))
    post_process_raw_results(cluster, experiment)
