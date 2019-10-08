# -*- encoding: utf8 -*-
from ..benchmark import *
# TODO Use the lue dataset for obtaining information, instead of the json files
from .weak_scaling_experiment import *
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

import json
import tempfile


def benchmark_meta_to_lue_json(
        benchmark_pathname,
        lue_pathname,
        cluster,
        benchmark,
        experiment):

    array_shape = experiment.array.shape()
    partition_shape = experiment.partition.shape()

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
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape
                                },
                                {
                                    "name": "partition_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [len(partition_shape)],
                                    "value": partition_shape
                                },
                                {
                                    "name": "worker_type",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [benchmark.worker.type]
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
        epoch,
        benchmark):

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

    nr_localities = benchmark_json["environment"]["nr_localities"]
    nr_threads = benchmark_json["environment"]["nr_threads"]
    nr_workers = \
        nr_localities if benchmark.worker.type == "node" else nr_threads

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
                                    "name": "nr_workers",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "value": [nr_workers]
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
    ### epoch = determine_epoch(cluster, benchmark, experiment)

    # Create a collection of benchmark indices where the indices are
    # sorted by start location in time
    benchmark_idxs, epoch = sort_benchmarks_by_time(
        cluster, benchmark, experiment)

    lue_dataset_pathname = experiment.result_pathname(
        cluster.name, "data", "lue")

    if os.path.exists(lue_dataset_pathname):
        os.remove(lue_dataset_pathname)

    metadata_written = False

    ### for benchmark_idx in range(benchmark.worker.nr_benchmarks()):
    for benchmark_idx in benchmark_idxs:

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)

        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, nr_workers, "json")
        assert os.path.exists(result_pathname), result_pathname

        if not metadata_written:
            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_meta_to_lue_json(
                    result_pathname, lue_json_file.name,
                    cluster, benchmark, experiment)
                import_lue_json(lue_json_file.name, lue_dataset_pathname)
            metadata_written = True

        with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
            benchmark_to_lue_json(
                result_pathname, lue_json_file.name, epoch, benchmark)
            import_lue_json(lue_json_file.name, lue_dataset_pathname)

    lue.assert_is_valid(lue_dataset_pathname)

    return lue_dataset_pathname


def meta_information_dataframe(
        lue_meta_information):

    assert \
        lue_meta_information.properties["kind"].value[:] == ["weak_scaling"]

    name = lue_meta_information.properties["name"].value[:]
    system_name = lue_meta_information.properties["system_name"].value[:]
    worker_type = lue_meta_information.properties["worker_type"].value[:]

    partition_shape = \
        lue_meta_information.properties["partition_shape"].value[:]
    assert len(partition_shape) == 1

    rank = len(partition_shape[0])

    meta_information = pd.DataFrame({
            "name": name,
            "system_name": system_name,
            "worker_type": worker_type,
        })
    partition_shape = pd.DataFrame(
        partition_shape,
        columns=["partition_shape_{}".format(i) for i in range(rank)])

    meta_information = pd.concat(
        [meta_information, partition_shape], axis=1)

    return meta_information


def measurement_dataframe(
        lue_measurement):

    array_shape = lue_measurement.properties["array_shape"].value[:]
    nr_workers = lue_measurement.properties["nr_workers"].value[:]
    duration = lue_measurement.properties["duration"].value[:]
    assert len(duration) == len(array_shape) == len(nr_workers)

    rank = array_shape.shape[1]
    count = duration.shape[1]

    # array shape per benchmark
    array_shape = pd.DataFrame(
        array_shape,
        columns=["array_shape_{}".format(i) for i in range(rank)])

    # count durations per benchmark
    duration = pd.DataFrame(
        duration,
        columns=["duration_{}".format(i) for i in range(count)])

    # nr_workers per benchmark
    nr_workers = pd.DataFrame(
        nr_workers,
        columns=["nr_workers"])

    assert (duration.index == array_shape.index).all()
    assert (duration.index == nr_workers.index).all()

    measurement = pd.concat([nr_workers, array_shape, duration], axis=1)

    return measurement


def post_process_raw_results(
        lue_dataset_pathname,
        plot_pathname,
        experiment):
    """
    Create plots and tables from raw benchmark results
    """
    lue_dataset = lue.open_dataset(lue_dataset_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]
    lue_meta_information = \
        lue_benchmark.collection_property_sets["meta_information"]
    lue_measurement = lue_benchmark.property_sets["measurement"]

    meta_information = meta_information_dataframe(lue_meta_information)
    name = meta_information.name[0]
    system_name = meta_information.system_name[0]
    worker_type = meta_information.worker_type[0]

    rank = lue_meta_information.properties["partition_shape"].value.shape[1]
    nr_benchmarks, count = lue_measurement.properties["duration"].value.shape

    measurement = measurement_dataframe(lue_measurement)


    # The time point at which the experiment was performed is the epoch
    # of the time domain used to store the durations
    lue_clock = lue_measurement.time_domain.clock
    assert lue_clock.nr_units == 1
    time_point_units = lue_clock.unit

    lue_epoch = lue_clock.epoch
    assert lue_epoch.kind == lue.Epoch.Kind.common_era
    assert lue_epoch.calendar == lue.Calendar.gregorian
    time_point = dateutil.parser.isoparse(lue_epoch.origin)

    # String containing time point in local time zone and conventions
    # time_point = time_point.astimezone(tzlocal.get_localzone()).strftime("%c")
    time_point = time_point.strftime("%c")

    nr_workers = measurement["nr_workers"]
    duration_labels = ["duration_{}".format(i) for i in range(count)]

    # t1 = duration using one worker
    t1 = measurement.loc[nr_workers == 1].filter(items=duration_labels)
    # t1 = [t1["duration_{}".format(i)][0] for i in range(count)]
    t1 = [t1.iat[0, i] for i in range(count)]

    for i in range(count):
        # Best case: duration stays constant with increasing the number of
        # workers and amount of work (and keeping the amount of work /
        # worker constant)
        # 100% parallel code, but without parallelization overhead
        measurement["linear_duration_{}".format(i)] = \
            [t1[i] for b in range(nr_benchmarks)]

        # Worst case: duration scales with number of workers
        # 100% serial code, but without parallelization overhead
        measurement["serial_duration_{}".format(i)] = t1[i] * nr_workers

        ### # slow_down = tn / linear_duration
        ### measurement["relative_slow_down_{}".format(i)] = \
        ###     (measurement["duration_{}".format(i)] / \
        ###     measurement["linear_duration_{}".format(i)]) - 1
        ### measurement["linear_relative_slow_down_{}".format(i)] = \
        ###     (measurement["linear_duration_{}".format(i)] / \
        ###     measurement["linear_duration_{}".format(i)]) - 1
        ### measurement["serial_relative_slow_down_{}".format(i)] = \
        ###     (measurement["serial_duration_{}".format(i)] / \
        ###     measurement["linear_duration_{}".format(i)]) - 1

        # efficiency = 100% * t1 / tn
        measurement["efficiency_{}".format(i)] = \
            100 * t1[i] / measurement["duration_{}".format(i)]
        measurement["linear_efficiency_{}".format(i)] = \
            100 * t1[i] / measurement["linear_duration_{}".format(i)]
        measurement["serial_efficiency_{}".format(i)] = \
            100 * t1[i] / measurement["serial_duration_{}".format(i)]


    # https://xkcd.com/color/rgb/
    serial_color = sns.xkcd_rgb["pale red"]
    linear_color = sns.xkcd_rgb["medium green"]
    actual_color = sns.xkcd_rgb["denim blue"]

    figure, axes = plt.subplots(
            nrows=2, ncols=1,
            figsize=(15, 10),
            sharex=True
        )  # Inches...

    # duration by nr_workers
    linear_duration = select_data_for_plot(
        measurement, "linear_duration", count)
    serial_duration = select_data_for_plot(
        measurement, "serial_duration", count)
    duration = select_data_for_plot(
        measurement, "duration", count)

    sns.lineplot(
        data=linear_duration, x="nr_workers", y="linear_duration",
        ax=axes[0], color=linear_color)
    sns.lineplot(
        data=serial_duration, x="nr_workers", y="serial_duration",
        ax=axes[0], color=serial_color)
    sns.lineplot(
        data=duration, x="nr_workers", y="duration",
        ax=axes[0], color=actual_color)
    axes[0].set_ylabel(u"duration ({}) ± 95% ci (count={})".format(
        time_point_units, count))
    axes[0].yaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda y, pos: format_duration(y)))
    axes[0].grid()

    ### # slow_down by nr_workers
    ### linear_relative_slow_down = select_data_for_plot(
    ###     measurement, "linear_relative_slow_down", count)
    ### serial_relative_slow_down = select_data_for_plot(
    ###     measurement, "serial_relative_slow_down", count)
    ### relative_slow_down = select_data_for_plot(
    ###     measurement, "relative_slow_down", count)

    ### sns.lineplot(
    ###     data=linear_relative_slow_down,
    ###     x="nr_workers", y="linear_relative_slow_down",
    ###     ax=axes[1], color=linear_color)
    ### sns.lineplot(
    ###     data=serial_relative_slow_down,
    ###     x="nr_workers", y="serial_relative_slow_down",
    ###     ax=axes[1], color=serial_color)
    ### sns.lineplot(
    ###     data=relative_slow_down,
    ###     x="nr_workers", y="relative_slow_down",
    ###     ax=axes[1], color=actual_color)

    ### max_relative_slow_down = measurement[
    ###         ["relative_slow_down_{}".format(i) for i in range(count)]
    ###     ].max().max()
    ### axes[1].set_ylim(None, 1.05 * max_relative_slow_down)
    ### axes[1].set_ylabel("relative slow down (-)")

    linear_efficiency = select_data_for_plot(
        measurement, "linear_efficiency", count)
    serial_efficiency = select_data_for_plot(
        measurement, "serial_efficiency", count)
    efficiency = select_data_for_plot(
        measurement, "efficiency", count)

    sns.lineplot(
        data=linear_efficiency, x="nr_workers", y="linear_efficiency",
        ax=axes[1], color=linear_color)
    sns.lineplot(
        data=serial_efficiency, x="nr_workers", y="serial_efficiency",
        ax=axes[1], color=serial_color)
    sns.lineplot(
        data=efficiency, x="nr_workers", y="efficiency",
        ax=axes[1], color=actual_color)
    axes[1].set_ylim(0, 110)
    axes[1].set_ylabel("efficiency (%)")
    axes[1].grid()

    axes[-1].xaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda x, pos: format_nr_workers(x)))
    axes[-1].set_xlabel("nr_workers ({})".format(worker_type))

    figure.legend(labels=["linear", "serial", "actual"])

    array_shape_per_worker = \
        lue_meta_information.properties["array_shape"].value[0]
    partition_shape = \
        lue_meta_information.properties["partition_shape"].value[0]

    figure.suptitle(
        "{}, {}, {}\n"
        "Weak scaling experiment on {} array per worker and {} partitions"
            .format(
                name,
                system_name,
                time_point,
                "x".join([str(extent) for extent in array_shape_per_worker]),
                "x".join([str(extent) for extent in partition_shape]),
            )
        )

    plt.tight_layout()
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
    job_scheduler = cluster_settings_json["job_scheduler"]
    assert job_scheduler in ["shell", "slurm"]

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)

    benchmark = Benchmark(benchmark_settings_json, cluster)
    experiment = WeakScalingExperiment(
        experiment_settings_json, command_pathname)

    lue_dataset_pathname = import_raw_results(cluster, benchmark, experiment)
    create_dot_graph(
        experiment.result_pathname(cluster.name, "data", "lue"),
        experiment.result_pathname(cluster.name, "graph", "pdf"))
    plot_pathname = experiment.result_pathname(cluster.name, "plot", "pdf")
    post_process_raw_results(lue_dataset_pathname, plot_pathname, experiment)
