# -*- encoding: utf8 -*-
from ..benchmark import *
# TODO Use the lue dataset for obtaining information, instead of the json files
from .strong_scaling_experiment import *
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
import matplotlib.ticker as ticker
import numpy as np
import pandas as pd
import seaborn as sns

import collections
import csv
from functools import reduce
import json
import re
import tempfile


def benchmark_meta_to_lue_json(
        benchmark_pathname,
        lue_pathname,
        cluster,
        benchmark,
        experiment):

    array_shape = experiment.array.shape
    partition_shape = experiment.partition.shape

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
                                    "name": "nr_time_steps",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "value": [experiment.nr_time_steps]
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


def read_performance_counters(
        counter_pathname):

    with open(counter_pathname, "r") as csv_file:
        reader = csv.reader(csv_file)
        field_names = next(reader)

    # HPX appends to CSVs... If this fails, remove CSVs and run experiment
    # again. Or remove earlier stuff from CSVs.
    array = np.loadtxt(counter_pathname, dtype=np.float64, unpack=True,
        delimiter=',', skiprows=1)

    if len(field_names) == 1:
        array = array.reshape(1, len(array))

    assert len(array) == len(field_names), "{}: {} != {}".format(
        counter_pathname, len(array), len(field_names))

    # # We are assuming uint64 values here
    # assert np.all(array >= 0)
    # assert np.all(array % 1 == 0)
    # array = array.astype(np.uint64)

    # Idle-rates are reported as 0.01%. Convert them to percentages.
    if "idle-rate" in field_names[0]:
        array /= 100.

    return field_names, array


def benchmark_to_lue_json(
        benchmark_pathname,
        lue_json_pathname,
        epoch,
        cluster,
        benchmark,
        experiment):

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

    # array_shape = list(benchmark_json["task"]["array_shape"])
    # partition_shape = list(benchmark_json["task"]["partition_shape"])

    nr_workers = benchmark_json["environment"]["nr_workers"]

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
        cluster.name, benchmark.scenario_name, "data", "lue")

    lue.create_dataset(lue_dataset_pathname, experiment.description)

    metadata_written = False

    for benchmark_idx in benchmark_idxs:

        nr_workers = benchmark.worker.nr_workers(benchmark_idx)

        result_pathname = experiment.benchmark_result_pathname(
            cluster.name, benchmark.scenario_name, nr_workers, "json")
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
                result_pathname, lue_json_file.name, epoch,
                cluster, benchmark, experiment)
            import_lue_json(lue_json_file.name, lue_dataset_pathname)

    lue.assert_is_valid(lue_dataset_pathname)

    return lue_dataset_pathname


def write_scaling_results(
        lue_dataset_pathname):

    lue_dataset = lue.open_dataset(lue_dataset_pathname, "w")
    count = lue_dataset.benchmark.measurement.duration.value.shape[1]

    lue_measurement = lue_dataset.benchmark.measurement
    lue_meta_information = lue_dataset.benchmark.meta_information

    # Write property-set to dataset containing the scalability information
    # - benchmark
    #     - scaling
    #         - (mean_duration)
    #         - (std_duration)
    #         - relative speed-up (+ mean, std)
    #         - efficiency (+ mean, std)
    #         - LUPS (+ mean, std)
    scaling_property_set = lue_dataset.benchmark.add_property_set(
        "scaling", lue_measurement.time_domain, lue_measurement.object_tracker)

    duration = lue_measurement.duration.value[:]
    nr_durations = len(duration)

    nr_workers = lue_measurement.nr_workers.value[:]
    nr_workers = nr_workers.reshape(len(nr_workers), 1)

    # Count durations, using one worker
    t1 = duration[0].astype(np.float64)

    # speed_up = t1 / tn
    relative_speed_up = t1 / duration

    relative_speed_up_property = scaling_property_set.add_property(
        "relative_speed_up", np.dtype(np.float64), shape=(count,),
        value_variability=lue.ValueVariability.variable,
        description="Relative speed-up: t1 / duration")
    relative_speed_up_property.value.expand(nr_durations)[:] = \
        relative_speed_up

    # efficiency = 100% * speed_up / nr_workers
    relative_efficiency = 100 * relative_speed_up / nr_workers

    relative_efficiency_property = scaling_property_set.add_property(
        "relative_efficiency", np.dtype(np.float64), shape=(count,),
        value_variability=lue.ValueVariability.variable,
        description="Relative efficiency: 100% * relative_speed_up / "
        "nr_workers")
    relative_efficiency_property.value.expand(nr_durations)[:] = \
        relative_efficiency

    # lups = nr_time_steps * nr_elements / duration
    # In the case of strong scaling, the nr_elements is
    # constant. Ideally, LUPS increases linearly with the nr_workers.
    nr_time_steps = lue_meta_information.nr_time_steps.value[0]
    array_shape = lue_meta_information.array_shape.value[0]
    nr_elements = reduce(lambda e1, e2: e1 * e2, array_shape)
    lups = nr_time_steps * nr_elements / duration

    lups_property = scaling_property_set.add_property(
        "lups", np.dtype(np.float64), shape=(count,),
        value_variability=lue.ValueVariability.variable,
        description="LUPS: nr_time_steps * nr_elements / "
        "duration")
    lups_property.value.expand(nr_durations)[:] = lups

    if count > 1:
        mean_duration = np.mean(duration, axis=1)
        std_duration = np.std(duration, axis=1)
        assert len(mean_duration) == len(std_duration) == nr_durations

        mean_relative_speed_up = np.mean(relative_speed_up, axis=1)
        std_relative_speed_up = np.std(relative_speed_up, axis=1)

        mean_relative_efficiency = np.mean(relative_efficiency, axis=1)
        std_relative_efficiency = np.std(relative_efficiency, axis=1)

        mean_lups = np.mean(lups, axis=1)
        std_lups = np.std(lups, axis=1)

        mean_duration_property = scaling_property_set.add_property(
            "mean_duration", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the mean duration of the {} "
                "experiments took."
                    .format(count))
        mean_duration_property.value.expand(nr_durations)[:] = mean_duration

        std_duration_property = scaling_property_set.add_property(
            "std_duration", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the standard deviation of the "
                "durations the {} experiments took."
                    .format(count))
        std_duration_property.value.expand(nr_durations)[:] = std_duration

        mean_relative_speed_up_property = scaling_property_set.add_property(
            "mean_relative_speed_up", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the mean of the relative "
                "speed-up of the {} experiments."
                    .format(count))
        mean_relative_speed_up_property.value.expand(nr_durations)[:] = \
            mean_relative_speed_up

        std_relative_speed_up_property = scaling_property_set.add_property(
            "std_relative_speed_up", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the standard deviation of the "
                "relative speed-ups of the {} experiments."
                    .format(count))
        std_relative_speed_up_property.value.expand(nr_durations)[:] = \
            std_relative_speed_up

        mean_relative_efficiency_property = scaling_property_set.add_property(
            "mean_relative_efficiency", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the mean of the relative "
                "efficiency of the {} experiments."
                    .format(count))
        mean_relative_efficiency_property.value.expand(nr_durations)[:] = \
            mean_relative_efficiency

        std_relative_efficiency_property = scaling_property_set.add_property(
            "std_relative_efficiency", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the standard deviation of the "
                "relative efficiency of the {} experiments."
                    .format(count))
        std_relative_efficiency_property.value.expand(nr_durations)[:] = \
            std_relative_efficiency

        mean_lups_property = scaling_property_set.add_property(
            "mean_lups", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the mean of the LUPS of the {} "
                "experiments."
                    .format(count))
        mean_lups_property.value.expand(nr_durations)[:] = mean_lups

        std_lups_property = scaling_property_set.add_property(
            "std_lups", np.dtype(np.float64), shape=(),
            value_variability=lue.ValueVariability.variable,
            description=
                "For a number of workers, the standard deviation of the "
                "LUPS of the {} experiments."
                    .format(count))
        std_lups_property.value.expand(nr_durations)[:] = std_lups


def post_process_raw_results(
        lue_dataset_pathname,
        plot_pathname):
    """
    Create plots and tables from raw benchmark results
    """
    lue_dataset = lue.open_dataset(lue_dataset_pathname)
    lue_meta_information = lue_dataset.benchmark.meta_information

    worker_type = lue_meta_information.worker_type.value[:][0]

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

    nr_workers = lue_dataset.benchmark.measurement.nr_workers.value[:]


    def annotate_plot(
            axis,
            y_label):
        axis.set_xlabel(u"workers ({})".format(worker_type))
        axis.set_xticks(nr_workers)
        axis.set_ylabel(y_label)
        axis.grid()


    def plot_duration(
            axis):

        if count == 1:
            duration = lue_dataset.benchmark.measurement.duration.value[:]
            y_label = u"duration ({})".format(time_point_units)
            plot_actual = lambda data: axis.plot(
                nr_workers, data,
                linewidth=default_linewidth,
                color=actual_color, marker="o")
        else:
            duration = lue_dataset.benchmark.scaling.mean_duration.value[:]
            error = lue_dataset.benchmark.scaling.std_duration.value[:]
            y_label= u"duration ({}) ± stddev (count={})".format(
                time_point_units, count)
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=default_linewidth,
                color=actual_color, marker="o")

        serial_duration = np.array([duration[0]
            for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, serial_duration, linewidth=default_linewidth,
            color=serial_color)

        linear_duration = np.array([duration[0] / nr_workers[n]
            for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, linear_duration, linewidth=default_linewidth,
            color=linear_color)

        plot_actual(duration)

        annotate_plot(axis, y_label)


    def plot_relative_speed_up(
            axis):

        if count == 1:
            relative_speed_up = \
                lue_dataset.benchmark.scaling.relative_speed_up.value[:]
            plot_actual = lambda data: axis.plot(
                nr_workers, data,
                linewidth=default_linewidth,
                color=actual_color, marker="o")
        else:
            relative_speed_up = \
                lue_dataset.benchmark.scaling.mean_relative_speed_up.value[:]
            error = \
                lue_dataset.benchmark.scaling.std_relative_speed_up.value[:]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=default_linewidth,
                color=actual_color, marker="o")

        serial_relative_speed_up = \
            np.array([relative_speed_up[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, serial_relative_speed_up,
            linewidth=default_linewidth,
            color=serial_color)

        linear_relative_speed_up = relative_speed_up[0] * nr_workers
        axis.plot(
            nr_workers, linear_relative_speed_up, linewidth=default_linewidth,
            color=linear_color)

        plot_actual(relative_speed_up)

        y_label= u"relative speed_up (-)"

        annotate_plot(axis, y_label)


    def plot_relative_efficiency(
            axis):

        if count == 1:
            relative_efficiency = \
                lue_dataset.benchmark.scaling.relative_efficiency.value[:]
            plot_actual = lambda data: axis.plot(
                nr_workers, data, linewidth=default_linewidth,
                color=actual_color, marker="o")
        else:
            relative_efficiency = \
                lue_dataset.benchmark.scaling.mean_relative_efficiency.value[:]
            error = \
                lue_dataset.benchmark.scaling.std_relative_efficiency.value[:]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=default_linewidth,
                color=actual_color, marker="o")

        serial_relative_efficiency = relative_efficiency[0] / nr_workers
        axis.plot(
            nr_workers, serial_relative_efficiency, linewidth=default_linewidth,
            color=serial_color)

        linear_relative_efficiency = \
            np.array([relative_efficiency[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, linear_relative_efficiency, linewidth=default_linewidth,
            color=linear_color)

        plot_actual(relative_efficiency)

        y_label= u"relative efficiency (%)"

        annotate_plot(axis, y_label)


    def plot_lups(
            axis):

        if count == 1:
            lups = \
                lue_dataset.benchmark.scaling.lups.value[:]
            plot_actual = lambda data: axis.plot(
                nr_workers, data, linewidth=default_linewidth,
                color=actual_color, marker="o")
        else:
            lups = \
                lue_dataset.benchmark.scaling.mean_lups.value[:]
            error = \
                lue_dataset.benchmark.scaling.std_lups.value[:]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=default_linewidth,
                color=actual_color, marker="o")

        serial_lups = \
            np.array([lups[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, serial_lups, linewidth=default_linewidth,
            color=serial_color)

        linear_lups = lups[0] * nr_workers
        axis.plot(
            nr_workers, linear_lups, linewidth=default_linewidth,
            color=linear_color)

        plot_actual(lups)

        y_label= u"LUPS"

        annotate_plot(axis, y_label)


    nr_plot_rows = 2
    nr_plot_cols = 2
    plot_width = 8  # Inches...
    plot_height = 6  # Inches...
    figure, axes = plt.subplots(
            nrows=nr_plot_rows, ncols=nr_plot_cols,
            figsize=(nr_plot_cols * plot_width, nr_plot_rows * plot_height),
            squeeze=False, sharex=False,
        )  # Inches...

    plot_duration(axes[0][0])
    plot_relative_speed_up(axes[0][1])
    plot_relative_efficiency(axes[1][0])
    plot_lups(axes[1][1])

    figure.legend(labels=["linear", "serial", "actual"])

    name = lue_meta_information.name.value[:][0]
    system_name = lue_meta_information.system_name.value[:][0]
    array_shape = lue_meta_information.array_shape.value[0]
    partition_shape = lue_meta_information.partition_shape.value[0]

    figure.suptitle(
        "{}, {}, {}\n"
        "Strong scaling experiment on {} array and {} partitions"
            .format(
                name,
                system_name,
                time_point,
                "x".join([str(extent) for extent in array_shape]),
                "x".join([str(extent) for extent in partition_shape]),
            )
        )

    plt.savefig(plot_pathname)


class PerformanceCounter(object):

    def __init__(self,
            object,
            instance,
            name,
            parameters=None):

        self.object = object
        self.instance = instance
        self.name = name
        self.parameters = parameters
        self.type = "{}/{}".format(self.object, self.name)

    def __str__(self):
        return \
            "PerformanceCounter(" \
                    "object={}, instance={}, name={}, parameters={})".format(
                self.object, self.instance, self.name, self.parameters)

    def full_name(self):
        # counter name: <object>{<instance>}/name@parameters
        assert self.parameters is None
        return "/{object}{{{instance}}}/{name}".format(
            object=self.object, instance=self.instance.full, name=self.name)


class PerformanceCounterGroup(object):

    def __init__(self,
            name):
        self.name = name
        self.counters = []
        self.ylim = (None, None)

    def __str__(self):
        return \
            "PerformanceCounterGroup(name={}, counters={})".format(
                self.name, self.counters)

    def __len__(self):
        return len(self.counters)


class PerformanceCounterInstance(object):

    def __init__(self,
            full):
        self.full = full

    def __str__(self):
        return \
            "PerformanceCounterInstance(full={})".format(self.full)


class SimplePerformanceCounterInstance(PerformanceCounterInstance):

    def __init__(self,
            parent,
            instance):
        PerformanceCounterInstance.__init__(self,
            "{}/{}".format(parent, instance))
        self.parent = parent
        self.instance = instance

    def __str__(self):
        return \
            "SimplePerformanceCounterInstance(parent={}, instance={})".format(
                self.parent, self.instance)


def parse_performance_counter_instance_name(
        name):

    # Turn name into a PerformanceCounterInstance object
    pattern = \
        "^(?P<parent>.+?)/(?P<instance>.+)$"
    match = re.match(pattern, name)
    assert match is not None, name

    parent = match.group("parent")
    instance = match.group("instance")

    # TODO Update for AggregatePerformanceCounterInstance when needed

    return SimplePerformanceCounterInstance(parent, instance)


def parse_performance_counter_name(
        name):

    # counter name: <object>{<instance>}/name@parameters
    pattern = \
        "^/(?P<object>.+){(?P<instance>.+)}/(?P<name>.+)(@(?P<parameters>.+))?$"
    match = re.match(pattern, name)
    assert match is not None, name

    object = match.group("object")
    instance = parse_performance_counter_instance_name(match.group("instance"))
    name = match.group("name")
    parameters = match.group("parameters")

    return PerformanceCounter(object, instance, name, parameters=parameters)


primary_namespace_services = [
    "route",
    "bind_gid",
    "resolve_gid",
    "unbind_gid",
    "increment_credit",
    "decrement_credit",
    "allocate",
    "begin_migration",
    "end_migration",
]


component_namespace_services = [
    "bind_prefix",
    "bind_name",
    "resolve_id",
    "unbind_name",
    "iterate_types",
    "get_component_typename",
    "num_localities_type",
]


locality_namespace_services = [
    "free",
    "localities",
    "num_localities",
    "num_threads",
    "resolve_locality",
]


symbol_namespace_services = [
    "bind",
    "resolve",
    "unbind",
    "iterate_names",
    "on_symbol_namespace_event",
]


agas_services = {
    "primary": primary_namespace_services,
    "component": component_namespace_services,
    "locality": locality_namespace_services,
    "symbol": symbol_namespace_services,
}


def classify_agas_service(
        service_name):

    for services_name in agas_services:
        if service_name in agas_services[services_name]:
            return services_name

    assert False, service_name


cache_statistics = [
    "entries",
    "evictions",
    "hits",
    "insertions",
    "misses",
]


full_cache_statistics = [
    "erase_entry",
    "get_entry",
    "insert_entry",
    "update_entry",
]


agas_statistics = {
    "cache": cache_statistics,
    "full_cache": full_cache_statistics,
}


def classify_agas_cache_statistic(
        statistic_name):

    for statistics_name in agas_statistics:
        if statistic_name in agas_statistics[statistics_name]:
            return statistics_name

    assert False, statistic_name


def classify_performance_counter(
        counter):

    group_name = counter.type

    # By default:
    # - Group counter per /object{instance.parent}/name
    # - Label counter by instance.instance
    # This implies that for each counter name a plot is made per locality,
    # and that each counter is named after the instance within the locality
    group_name = "{}{{{}}}/{}".format(
        counter.object, counter.instance.parent, counter.name)
    counter_label = counter.instance.instance

    # Exceptions to the default grouping rule
    if counter.object == "agas":
        if counter.name.startswith("count/cache"):
            # - Group per instance
            # - Group per statistic kind
            name, _, statistic_name = counter.name.rpartition("/")
            statistic_kind = classify_agas_cache_statistic(statistic_name)
            group_name = "agas{{{}}}/{}/<{}>".format(
                counter.instance.full, name, statistic_kind)
            counter_label = statistic_name
        elif counter.name.startswith("time/cache"):
            name, _, statistic = counter.name.rpartition("/")
            group_name = "agas{{{}}}/{}".format(counter.instance.full, name)
            counter_label = statistic
        elif counter.name.startswith("count"):
            # - Group per instance
            # - Group per AGAS service kind
            agas_service_name = counter.name.split("/")[1]
            agas_services_name = classify_agas_service(agas_service_name)
            group_name = "agas{{{}}}/count/<{}>".format(
                counter.instance.full, agas_services_name)
            counter_label = agas_service_name

    return group_name, counter_label


def group_performance_counters(
        counter_names):
    """
    Given the names of the counters produced, create groups that are
    useful to combine in plots. Also add some information to improve
    the plots, like limits in y.

    This function contains counter-specific knowledge.
    """
    # A dict with per group a high level name, also used as key, and a
    # group instance containing all information about the group
    groups = collections.OrderedDict()

    # Counters are grouped according to the object. Within this group,
    # counters are also grouped.

    for counter_name in counter_names:
        # Classify counter and add to group. Create group if not
        # existant yet.
        counter = parse_performance_counter_name(counter_name)

        # Counters related to the same object
        object_groups = groups.setdefault(
            counter.object, collections.OrderedDict())

        group_name, counter_label = classify_performance_counter(counter)
        counter_group = object_groups.setdefault(
            group_name, PerformanceCounterGroup(group_name))
        counter.label = counter_label
        counter_group.counters.append(counter)

    for names, object_groups in groups.items():
        for name, group in object_groups.items():
            if "idle-rate" in name:
                group.ylim = (0, 100)

    return groups


def plot_performance_counters(
        counter_pathname):

    counter_dirname, counter_basename = os.path.split(counter_pathname)

    # TODO
    # - Create dir per object
    # - Write plot per group
    # - Make sure each individual plot contains all information

    # Read data in numpy arrays and translate into data frame
    counter_names, counter_values = read_performance_counters(counter_pathname)
    assert len(counter_values) == len(counter_names), "{} != {}".format(
        len(counter_values), len(counter_names))
    data_frame = pd.DataFrame(dict(zip(counter_names, counter_values)))

    plot_pathnames = []

    groups_per_object = group_performance_counters(counter_names)

    for object_name in groups_per_object:
        # object_name: agas, threads, parcels, parcelqueue, runtime,
        #     threadqueue, ...
        counter_groups = groups_per_object[object_name]

        directory_pathname = os.path.join(counter_dirname, object_name)

        if not os.path.isdir(directory_pathname):
            os.mkdir(directory_pathname)

        plot_pathname = os.path.join(
            directory_pathname,
            "{}.pdf".format(os.path.splitext(counter_basename)[0]))
        assert not plot_pathname in plot_pathnames
        plot_pathnames.append(plot_pathname)

        nr_plots = len(counter_groups)

        figure, axes = plt.subplots(
                nrows=nr_plots, ncols=1,
                figsize=(15, 5 * nr_plots),
                squeeze=False, sharex=False
            )  # Inches...

        for p in range(nr_plots):
            group_name = list(counter_groups.keys())[p]
            counter_group = counter_groups[group_name]

            for counter in counter_group.counters:
                sns.lineplot(
                    data=data_frame[counter.full_name()], ax=axes[p, 0])

            axes[p, 0].set_ylabel(counter_group.name)
            axes[p, 0].grid()
            axes[p, 0].set_title(counter_group.name)
            axes[p, 0].set_xlabel(
                "interval (x {} milliseconds)".format("todo"))
            axes[p, 0].set_ylim(*counter_group.ylim)

            if len(counter_group) <= 10:
                axes[p, 0].legend(
                    labels=[
                        counter.label for counter in counter_group.counters])

        # plt.tight_layout()
        plt.savefig(plot_pathname)
        plt.close(figure)


def post_process_performance_counters(
        lue_dataset_pathname,
        scenario_name,
        experiment):

    # Iterate over all files containing performance counter information
    # and create plots

    lue_dataset = lue.open_dataset(lue_dataset_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]

    lue_meta_information = \
        lue_benchmark.collection_property_sets["meta_information"]
    system_name = lue_meta_information.properties["system_name"].value[:][0]

    lue_measurement = lue_benchmark.property_sets["measurement"]
    nr_workers = lue_measurement.properties["nr_workers"].value[:]

    for nr_workers_ in nr_workers:
        counter_pathname = experiment.benchmark_result_pathname(
            system_name, scenario_name, "counter-{}".format(nr_workers_), "csv")
        assert os.path.exists(counter_pathname), counter_pathname

        plot_performance_counters(counter_pathname)


def post_process_results(
        cluster_settings_json,
        benchmark_settings_json,
        experiment_settings_json,
        command_pathname):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function
    """
    cluster = Cluster(cluster_settings_json)
    benchmark = Benchmark(benchmark_settings_json, cluster)
    experiment = StrongScalingExperiment(
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
    post_process_raw_results(lue_dataset_pathname, plot_pathname)

    performance_counters_available = benchmark.hpx is not None and \
        benchmark.hpx.performance_counters is not None

    if performance_counters_available:
        post_process_performance_counters(
            lue_dataset_pathname, benchmark.scenario_name, experiment)
