# -*- encoding: utf8 -*-
from ..benchmark import *
# TODO Use the lue dataset for obtaining information, instead of the json files
from .strong_scaling_experiment import *
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

import collections
import csv
import json
import re
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


def read_performance_counters(
        counter_pathname):

    with open(counter_pathname, "r") as csv_file:
        reader = csv.reader(csv_file)
        field_names = next(reader)

    # HPX appends to CSVs... If this fails, remove CSVs and run experiment
    # again. Or remove earlier stuff from CSVs.
    array = np.loadtxt(counter_pathname, dtype=np.float64, unpack=True,
        delimiter=',', skiprows=1)

    assert len(array) == len(field_names), "{}: {} != {}".format(
        counter_pathname, len(array), len(field_names))

    # # We are assuming uint64 values here
    # assert np.all(array >= 0)
    # assert np.all(array % 1 == 0)
    # array = array.astype(np.uint64)

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
                result_pathname, lue_json_file.name, epoch,
                cluster, benchmark, experiment)
            import_lue_json(lue_json_file.name, lue_dataset_pathname)

    lue.assert_is_valid(lue_dataset_pathname)

    return lue_dataset_pathname


def meta_information_dataframe(
        lue_meta_information):

    assert \
        lue_meta_information.properties["kind"].value[:] == ["strong_scaling"]

    name = lue_meta_information.properties["name"].value[:]
    system_name = lue_meta_information.properties["system_name"].value[:]
    worker_type = lue_meta_information.properties["worker_type"].value[:]

    array_shape = lue_meta_information.properties["array_shape"].value[:]
    assert len(array_shape) == 1
    partition_shape = \
        lue_meta_information.properties["partition_shape"].value[:]
    assert len(partition_shape) == 1

    rank = len(partition_shape[0])

    meta_information = pd.DataFrame({
            "name": name,
            "system_name": system_name,
            "worker_type": worker_type,
        })
    array_shape = pd.DataFrame(
        array_shape,
        columns=["array_shape_{}".format(i) for i in range(rank)])
    partition_shape = pd.DataFrame(
        partition_shape,
        columns=["partition_shape_{}".format(i) for i in range(rank)])

    meta_information = pd.concat(
        [meta_information, array_shape, partition_shape], axis=1)

    return meta_information


def measurement_dataframe(
        lue_measurement):

    nr_workers = lue_measurement.properties["nr_workers"].value[:]
    duration = lue_measurement.properties["duration"].value[:]
    assert len(duration) == len(nr_workers)

    count = duration.shape[1]

    # count durations per benchmark
    duration = pd.DataFrame(
        duration,
        columns=["duration_{}".format(i) for i in range(count)])

    # nr_workers per benchmark
    nr_workers = pd.DataFrame(
        nr_workers,
        columns=["nr_workers"])

    assert (duration.index == nr_workers.index).all()

    measurement = pd.concat([nr_workers, duration], axis=1)

    ### # Sort measurements by the number of workers
    ### measurement.sort_values(by=["nr_workers"], inplace=True)

    return measurement


def post_process_raw_results(
        lue_dataset_pathname,
        plot_pathname):
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

    rank = lue_meta_information.properties["array_shape"].value.shape[1]
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
        # Best case: duration scales perfect with the number of workers
        # 100% parallel code, but without parallelization overhead
        measurement["linear_duration_{}".format(i)] = t1[i] / nr_workers

        # Worst case: duration does not scale with the number of threads
        # 100% serial code, but without parallelization overhead
        measurement["serial_duration_{}".format(i)] = \
            [t1[i] for b in range(nr_benchmarks)]

        # speed_up = t1 / tn
        measurement["relative_speed_up_{}".format(i)] = \
            t1[i] / measurement["duration_{}".format(i)]
        measurement["linear_relative_speed_up_{}".format(i)] = \
            t1[i] / measurement["linear_duration_{}".format(i)]
        measurement["serial_relative_speed_up_{}".format(i)] = \
            t1[i] / measurement["serial_duration_{}".format(i)]

        # efficiency = 100% * speed_up / nr_workers
        measurement["efficiency_{}".format(i)] = 100 * \
            measurement["relative_speed_up_{}".format(i)] / nr_workers
        measurement["linear_efficiency_{}".format(i)] = 100 * \
            measurement["linear_relative_speed_up_{}".format(i)] / nr_workers
        measurement["serial_efficiency_{}".format(i)] = 100 * \
            measurement["serial_relative_speed_up_{}".format(i)] / nr_workers


    # https://xkcd.com/color/rgb/
    serial_color = sns.xkcd_rgb["pale red"]
    linear_color = sns.xkcd_rgb["medium green"]
    actual_color = sns.xkcd_rgb["denim blue"]

    figure, axes = plt.subplots(
            nrows=3, ncols=1,
            figsize=(15, 15),
            sharex=True
        )  # Inches...

    # duration by nr_workers
    duration = select_data_for_plot(
        measurement, "duration", count)
    linear_duration = select_data_for_plot(
        measurement, "linear_duration", count)
    serial_duration = select_data_for_plot(
        measurement, "serial_duration", count)

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

    # speed_up by nr_workers
    relative_speed_up = select_data_for_plot(
        measurement, "relative_speed_up", count)
    linear_relative_speed_up = select_data_for_plot(
        measurement, "linear_relative_speed_up", count)
    serial_relative_speed_up = select_data_for_plot(
        measurement, "serial_relative_speed_up", count)

    sns.lineplot(
        data=linear_relative_speed_up,
        x="nr_workers", y="linear_relative_speed_up",
        ax=axes[1], color=linear_color)
    sns.lineplot(
        data=serial_relative_speed_up,
        x="nr_workers", y="serial_relative_speed_up",
        ax=axes[1], color=serial_color)
    sns.lineplot(
        data=relative_speed_up,
        x="nr_workers", y="relative_speed_up",
        ax=axes[1], color=actual_color)
    axes[1].set_ylabel("relative speed up (-)")
    axes[1].grid()

    # efficiency by nr_workers
    efficiency = select_data_for_plot(
        measurement, "efficiency", count)
    linear_efficiency = select_data_for_plot(
        measurement, "linear_efficiency", count)
    serial_efficiency = select_data_for_plot(
        measurement, "serial_efficiency", count)

    sns.lineplot(
        data=linear_efficiency, x="nr_workers", y="linear_efficiency",
        ax=axes[2], color=linear_color)
    sns.lineplot(
        data=serial_efficiency, x="nr_workers", y="serial_efficiency",
        ax=axes[2], color=serial_color)
    sns.lineplot(
        data=efficiency, x="nr_workers", y="efficiency",
        ax=axes[2], color=actual_color)
    axes[2].set_ylim(0, 110)
    axes[2].set_ylabel("efficiency (%)")
    axes[2].grid()

    axes[-1].xaxis.set_major_formatter(
        ticker.FuncFormatter(
            lambda x, pos: format_nr_workers(x)))
    axes[-1].set_xlabel("nr_workers ({})".format(worker_type))

    figure.legend(labels=["linear", "serial", "actual"])

    array_shape = lue_meta_information.properties["array_shape"].value[0]
    partition_shape = \
        lue_meta_information.properties["partition_shape"].value[0]

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

    # for names, object_groups in groups.items():
    #     for name, group in object_groups.items():
    #         if "idle-rate" in name:
    #             group.ylim = (0, 100)

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
                squeeze=False, sharex=True
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
            axes[p, 0].set_ylim(*counter_group.ylim)

            if len(counter_group) <= 10:
                axes[p, 0].legend(
                    labels=[
                        counter.label for counter in counter_group.counters])

        axes[-1, 0].set_xlabel("interval (x {} milliseconds)".format("todo"))

        plt.tight_layout()
        plt.savefig(plot_pathname)
        plt.close(figure)


def post_process_performance_counters(
        lue_dataset_pathname,
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
            system_name, "counter-{}".format(nr_workers_), "csv")
        assert os.path.exists(counter_pathname), counter_pathname

        plot_performance_counters(counter_pathname)


def post_process_results(
        cluster_settings_json,
        benchmark_settings_json,  # Not used atm...
        experiment_settings_json,
        command_pathname):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function
    """
    job_scheduler = cluster_settings_json["job_scheduler"]
    assert job_scheduler in ["shell", "slurm"]

    if job_scheduler == "slurm":
        cluster = SlurmCluster(cluster_settings_json)
    elif job_scheduler == "shell":
        cluster = ShellCluster(cluster_settings_json)

    benchmark = Benchmark(benchmark_settings_json, cluster)
    experiment = StrongScalingExperiment(
        experiment_settings_json, command_pathname)

    lue_dataset_pathname = import_raw_results(cluster, benchmark, experiment)
    create_dot_graph(
        experiment.result_pathname(cluster.name, "data", "lue"),
        experiment.result_pathname(cluster.name, "graph", "pdf"))

    plot_pathname = experiment.result_pathname(cluster.name, "plot", "pdf")
    post_process_raw_results(lue_dataset_pathname, plot_pathname)

    performance_counters_available = benchmark.hpx is not None and \
        benchmark.hpx.performance_counters is not None

    if performance_counters_available:
        post_process_performance_counters(lue_dataset_pathname, experiment)
