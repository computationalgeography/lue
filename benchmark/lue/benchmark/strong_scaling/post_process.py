# -*- encoding: utf8 -*-
from .strong_scaling_experiment import StrongScalingExperiment
from .. import dataset
from .. import job
from .. import plot
from .. import util

import lue
import dateutil.parser
import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np


def post_process_raw_results(
        lue_dataset,
        plot_pathname):
    """
    Create plots and tables from raw benchmark results
    """
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
                linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")
        else:
            duration = lue_dataset.benchmark.scaling.mean_duration.value[:]
            error = lue_dataset.benchmark.scaling.std_duration.value[:]
            y_label= u"duration ({}) ± stddev (count={})".format(
                time_point_units, count)
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")

        serial_duration = np.array([duration[0]
            for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, serial_duration, linewidth=plot.default_linewidth,
            color=plot.serial_color)

        linear_duration = np.array([duration[0] / nr_workers[n]
            for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, linear_duration, linewidth=plot.default_linewidth,
            color=plot.linear_color)

        plot_actual(duration)

        annotate_plot(axis, y_label)


    def plot_relative_speed_up(
            axis):

        if count == 1:
            relative_speed_up = \
                lue_dataset.benchmark.scaling.relative_speed_up.value[:]
            plot_actual = lambda data: axis.plot(
                nr_workers, data,
                linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")
        else:
            relative_speed_up = \
                lue_dataset.benchmark.scaling.mean_relative_speed_up.value[:]
            error = \
                lue_dataset.benchmark.scaling.std_relative_speed_up.value[:]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")

        serial_relative_speed_up = \
            np.array([relative_speed_up[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, serial_relative_speed_up,
            linewidth=plot.default_linewidth,
            color=plot.serial_color)

        linear_relative_speed_up = relative_speed_up[0] * nr_workers
        axis.plot(
            nr_workers, linear_relative_speed_up, linewidth=plot.default_linewidth,
            color=plot.linear_color)

        plot_actual(relative_speed_up)

        y_label= u"relative speed_up (-)"

        annotate_plot(axis, y_label)


    def plot_relative_efficiency(
            axis):

        if count == 1:
            relative_efficiency = \
                lue_dataset.benchmark.scaling.relative_efficiency.value[:]
            plot_actual = lambda data: axis.plot(
                nr_workers, data, linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")
        else:
            relative_efficiency = \
                lue_dataset.benchmark.scaling.mean_relative_efficiency.value[:]
            error = \
                lue_dataset.benchmark.scaling.std_relative_efficiency.value[:]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")

        serial_relative_efficiency = relative_efficiency[0] / nr_workers
        axis.plot(
            nr_workers, serial_relative_efficiency, linewidth=plot.default_linewidth,
            color=plot.serial_color)

        linear_relative_efficiency = \
            np.array([relative_efficiency[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, linear_relative_efficiency, linewidth=plot.default_linewidth,
            color=plot.linear_color)

        plot_actual(relative_efficiency)

        y_label= u"relative efficiency (%)"

        annotate_plot(axis, y_label)


    def plot_lups(
            axis):

        if count == 1:
            lups = \
                lue_dataset.benchmark.scaling.lups.value[:]
            plot_actual = lambda data: axis.plot(
                nr_workers, data, linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")
        else:
            lups = \
                lue_dataset.benchmark.scaling.mean_lups.value[:]
            error = \
                lue_dataset.benchmark.scaling.std_lups.value[:]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers, y=data, yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color, marker="o")

        serial_lups = \
            np.array([lups[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, serial_lups, linewidth=plot.default_linewidth,
            color=plot.serial_color)

        linear_lups = lups[0] * nr_workers
        axis.plot(
            nr_workers, linear_lups, linewidth=plot.default_linewidth,
            color=plot.linear_color)

        plot_actual(lups)

        y_label= u"throughput (LUPS)"

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
    scenario_name = lue_meta_information.scenario_name.value[:][0]
    array_shape = lue_meta_information.array_shape.value[0]
    partition_shape = lue_meta_information.partition_shape.value[0]

    figure.suptitle(
        "{}, {}, {}\n"
        "Strong scaling experiment on {} array and {} partitions ({})"
            .format(
                name,
                system_name,
                time_point,
                "x".join([str(extent) for extent in array_shape]),
                "x".join([str(extent) for extent in partition_shape]),
                scenario_name,
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

        result = "/{object}{{{instance}}}/{name}".format(
            object=self.object, instance=self.instance.full, name=self.name)

        if self.parameters:
            result = "{}@{}".format(result, self.parameters)

        return result


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

    result = SimplePerformanceCounterInstance(parent, instance)

    # assert result.full_name() == name, \
    #     "{} != {}".format(result.full_name(), name)

    return result


def parse_performance_counter_name(
        name):

    # counter name: <object>{<instance>}/name@parameters
    pattern = \
        "^/(?P<object>.+){(?P<instance>.+)}/(?P<name>[^@]+)(@(?P<parameters>.+))?$"
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

    # CSV file containing the counter results
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
        lue_dataset,
        scenario_name,
        experiment):

    # Iterate over all files containing performance counter information
    # and create plots

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
        results_prefix):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function
    """
    lue_dataset = job.open_lue_dataset(results_prefix, "r")
    cluster, benchmark, experiment = dataset.read_benchmark_settings(
        lue_dataset, StrongScalingExperiment)

    util.create_dot_graph(
        lue_dataset.pathname,
        experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "graph", "pdf"))
    plot_pathname = experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "plot", "pdf")

    post_process_raw_results(lue_dataset, plot_pathname)

    performance_counters_available = benchmark.hpx is not None and \
        benchmark.hpx.performance_counters is not None

    if performance_counters_available:
        post_process_performance_counters(
            lue_dataset, benchmark.scenario_name)
