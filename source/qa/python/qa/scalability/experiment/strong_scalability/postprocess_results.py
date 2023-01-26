from .configuration import Configuration
from .experiment import Experiment
from .. import dataset
from .. import job
from lue.qa.scalability.core import math
from lue.qa.scalability.core import plot
from lue.qa.scalability.core import process
import lue.data_model as ldm
import dateutil.parser
import matplotlib.pyplot as plt
import numpy as np

# TODO perf countrs
# from tqdm import trange
# import collections
# import os.path
# import re


def post_process_raw_results(lue_dataset, result_prefix, plot_pathname):
    """
    Create plots and tables from raw benchmark results
    """
    lue_meta_information = lue_dataset.benchmark.meta_information

    worker_type = lue_meta_information.worker_type.value[:][0]

    lue_measurement = lue_dataset.benchmark.measurement

    count = lue_measurement.duration.value.shape[1]

    # The time point at which the experiment was performed is the epoch
    # of the time domain used to store the durations
    lue_clock = lue_measurement.time_domain.clock
    assert lue_clock.nr_units == 1
    time_point_units = lue_clock.unit

    lue_epoch = lue_clock.epoch
    assert lue_epoch.kind == ldm.Epoch.Kind.common_era
    assert lue_epoch.calendar == ldm.Calendar.gregorian
    time_point = dateutil.parser.isoparse(lue_epoch.origin)

    # String containing time point in local time zone and conventions
    # time_point = time_point.astimezone(tzlocal.get_localzone()).strftime("%c")
    time_point = time_point.strftime("%c")

    nr_workers = lue_measurement.nr_workers.value[:]

    # Results are sorted by time not by nr_workers. All values to be
    # plotted need to be sorted, by nr_workers.
    sort_idxs = np.argsort(nr_workers)
    nr_workers = nr_workers[sort_idxs]
    assert math.is_monotonically_increasing(nr_workers), nr_workers
    assert nr_workers[0] == 1, nr_workers

    lue_scaling = lue_dataset.benchmark.scaling

    def annotate_plot(axis, y_label):

        axis.set_xlabel("workers ({})".format(worker_type))
        axis.set_xticks(nr_workers)
        axis.set_ylabel(y_label)
        axis.grid()

    def plot_duration(axis):

        if count == 1:
            duration = lue_measurement.duration.value[:][sort_idxs]
            y_label = "duration ({})".format(time_point_units)
            plot_actual = lambda data: axis.plot(
                nr_workers,
                data,
                linewidth=plot.default_linewidth,
                color=plot.actual_color,
                marker="o",
            )
        else:
            duration = lue_scaling.mean_duration.value[:][sort_idxs]
            error = lue_scaling.std_duration.value[:][sort_idxs]
            y_label = "duration ({}) ± stddev (count={})".format(
                time_point_units, count
            )
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers,
                y=data,
                yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color,
                marker="o",
            )

        serial_duration = np.array([duration[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers,
            serial_duration,
            linewidth=plot.default_linewidth,
            color=plot.serial_color,
        )

        linear_duration = np.array(
            [duration[0] / nr_workers[n] for n in range(len(nr_workers))]
        )
        axis.plot(
            nr_workers,
            linear_duration,
            linewidth=plot.default_linewidth,
            color=plot.linear_color,
        )

        plot_actual(duration)

        annotate_plot(axis, y_label)

    def plot_relative_speed_up(axis):

        if count == 1:
            relative_speed_up = lue_scaling.relative_speed_up.value[:][sort_idxs]
            plot_actual = lambda data: axis.plot(
                nr_workers,
                data,
                linewidth=plot.default_linewidth,
                color=plot.actual_color,
                marker="o",
            )
        else:
            relative_speed_up = lue_scaling.mean_relative_speed_up.value[:][sort_idxs]
            error = lue_scaling.std_relative_speed_up.value[:][sort_idxs]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers,
                y=data,
                yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color,
                marker="o",
            )

        serial_relative_speed_up = np.array(
            [relative_speed_up[0] for n in range(len(nr_workers))]
        )
        axis.plot(
            nr_workers,
            serial_relative_speed_up,
            linewidth=plot.default_linewidth,
            color=plot.serial_color,
        )

        linear_relative_speed_up = relative_speed_up[0] * nr_workers
        axis.plot(
            nr_workers,
            linear_relative_speed_up,
            linewidth=plot.default_linewidth,
            color=plot.linear_color,
        )

        plot_actual(relative_speed_up)

        y_label = "relative speed_up (-)"

        annotate_plot(axis, y_label)

    def plot_relative_efficiency(axis):

        if count == 1:
            relative_efficiency = lue_scaling.relative_efficiency.value[:][sort_idxs]
            plot_actual = lambda data: axis.plot(
                nr_workers,
                data,
                linewidth=plot.default_linewidth,
                color=plot.actual_color,
                marker="o",
            )
        else:
            relative_efficiency = lue_scaling.mean_relative_efficiency.value[:][
                sort_idxs
            ]
            error = lue_scaling.std_relative_efficiency.value[:][sort_idxs]
            plot_actual = lambda data: axis.errorbar(
                x=nr_workers,
                y=data,
                yerr=error,
                linewidth=plot.default_linewidth,
                color=plot.actual_color,
                marker="o",
            )

        serial_relative_efficiency = relative_efficiency[0] / nr_workers
        axis.plot(
            nr_workers,
            serial_relative_efficiency,
            linewidth=plot.default_linewidth,
            color=plot.serial_color,
        )

        linear_relative_efficiency = np.array(
            [relative_efficiency[0] for n in range(len(nr_workers))]
        )
        axis.plot(
            nr_workers,
            linear_relative_efficiency,
            linewidth=plot.default_linewidth,
            color=plot.linear_color,
        )

        plot_actual(relative_efficiency)

        y_label = "relative efficiency (%)"

        annotate_plot(axis, y_label)

    # def plot_lups(
    #         axis):

    #     if count == 1:
    #         lups = lue_scaling.lups.value[:][sort_idxs]
    #         plot_actual = lambda data: axis.plot(
    #             nr_workers, data, linewidth=plot.default_linewidth,
    #             color=plot.actual_color, marker="o")
    #     else:
    #         lups = lue_scaling.mean_lups.value[:][sort_idxs]
    #         error = lue_scaling.std_lups.value[:][sort_idxs]
    #         plot_actual = lambda data: axis.errorbar(
    #             x=nr_workers, y=data, yerr=error,
    #             linewidth=plot.default_linewidth,
    #             color=plot.actual_color, marker="o")

    #     serial_lups = np.array([lups[0] for n in range(len(nr_workers))])
    #     axis.plot(
    #         nr_workers, serial_lups, linewidth=plot.default_linewidth,
    #         color=plot.serial_color)

    #     linear_lups = lups[0] * nr_workers
    #     axis.plot(
    #         nr_workers, linear_lups, linewidth=plot.default_linewidth,
    #         color=plot.linear_color)

    #     plot_actual(lups)

    #     y_label= u"throughput (LUPS)"

    #     annotate_plot(axis, y_label)

    nr_plot_rows = 2
    nr_plot_cols = 2
    plot_width = 8  # Inches...
    plot_height = 6  # Inches...
    figure, axes = plt.subplots(
        nrows=nr_plot_rows,
        ncols=nr_plot_cols,
        figsize=(nr_plot_cols * plot_width, nr_plot_rows * plot_height),
        squeeze=False,
        sharex=False,
    )  # Inches...

    plot_duration(axes[0][0])
    plot_relative_speed_up(axes[0][1])
    plot_relative_efficiency(axes[1][0])
    # plot_lups(axes[1][1])
    axes[1, 1].axis("off")

    figure.legend(labels=["serial", "linear", "actual"])

    name = lue_meta_information.name.value[:][0]
    system_name = lue_meta_information.system_name.value[:][0]
    scenario_name = lue_meta_information.scenario_name.value[:][0]
    array_shape = lue_meta_information.array_shape.value[0]
    partition_shape = lue_meta_information.partition_shape.value[0]

    figure.suptitle(
        "{}, {}, {}\n"
        "Strong scalability experiment on {} array and {} partitions ({})".format(
            name,
            system_name,
            time_point,
            "x".join([str(extent) for extent in array_shape]),
            "x".join([str(extent) for extent in partition_shape]),
            scenario_name,
        )
    )

    plt.savefig(plot_pathname, bbox_inches="tight")


# class PerformanceCounter(object):
#
#     def __init__(self,
#             object,
#             instance,
#             name,
#             parameters=None):
#
#         self.object = object
#         self.instance = instance
#         self.name = name
#         self.parameters = parameters
#         self.type = "{}/{}".format(self.object, self.name)
#
#     def __str__(self):
#         return \
#             "PerformanceCounter(" \
#                     "object={}, instance={}, name={}, parameters={})".format(
#                 self.object, self.instance, self.name, self.parameters)
#
#     @property
#     def full_name(self):
#         # counter name: <object>{<instance>}/name@parameters
#
#         result = "/{object}{{{instance}}}/{name}".format(
#             object=self.object, instance=self.instance.full, name=self.name)
#
#         if self.parameters:
#             result = "{}@{}".format(result, self.parameters)
#
#         return result
#
#
# class PerformanceCounterGroup(object):
#
#     def __init__(self,
#             name):
#         self.name = name
#         self.counters = []
#         self.ylim = (None, None)
#
#     def __str__(self):
#         return \
#             "PerformanceCounterGroup(name={}, counters={})".format(
#                 self.name, self.counters)
#
#     def __len__(self):
#         return len(self.counters)
#
#
# class PerformanceCounterInstance(object):
#
#     def __init__(self,
#             full):
#         self.full = full
#
#     def __str__(self):
#         return \
#             "PerformanceCounterInstance(full={})".format(self.full)
#
#
# class SimplePerformanceCounterInstance(PerformanceCounterInstance):
#
#     def __init__(self,
#             parent,
#             instance):
#         PerformanceCounterInstance.__init__(self,
#             "{}/{}".format(parent, instance))
#         self.parent = parent
#         self.instance = instance
#
#     def __str__(self):
#         return \
#             "SimplePerformanceCounterInstance(parent={}, instance={})".format(
#                 self.parent, self.instance)
#
#
# def parse_performance_counter_instance_name(
#         name):
#
#     # Turn name into a PerformanceCounterInstance object
#     pattern = \
#         "^(?P<parent>.+?)/(?P<instance>.+)$"
#     match = re.match(pattern, name)
#     assert match is not None, name
#
#     parent = match.group("parent")
#     instance = match.group("instance")
#
#     # TODO Update for AggregatePerformanceCounterInstance when needed
#
#     result = SimplePerformanceCounterInstance(parent, instance)
#
#     # assert result.full_name() == name, \
#     #     "{} != {}".format(result.full_name(), name)
#
#     return result
#
#
# def parse_performance_counter_name(
#         name):
#
#     # counter name: <object>{<instance>}/name@parameters
#     pattern = \
#         "^/(?P<object>.+){(?P<instance>.+)}/(?P<name>[^@]+)(@(?P<parameters>.+))?$"
#     match = re.match(pattern, name)
#     assert match is not None, name
#
#     object = match.group("object")
#     instance = parse_performance_counter_instance_name(match.group("instance"))
#     name = match.group("name")
#     parameters = match.group("parameters")
#
#     return PerformanceCounter(object, instance, name, parameters=parameters)
#
#
# primary_namespace_services = [
#     "route",
#     "bind_gid",
#     "resolve_gid",
#     "unbind_gid",
#     "increment_credit",
#     "decrement_credit",
#     "allocate",
#     "begin_migration",
#     "end_migration",
# ]
#
#
# component_namespace_services = [
#     "bind_prefix",
#     "bind_name",
#     "resolve_id",
#     "unbind_name",
#     "iterate_types",
#     "get_component_typename",
#     "num_localities_type",
# ]
#
#
# locality_namespace_services = [
#     "free",
#     "localities",
#     "num_localities",
#     "num_threads",
#     "resolve_locality",
# ]
#
#
# symbol_namespace_services = [
#     "bind",
#     "resolve",
#     "unbind",
#     "iterate_names",
#     "on_symbol_namespace_event",
# ]
#
#
# agas_services = {
#     "primary": primary_namespace_services,
#     "component": component_namespace_services,
#     "locality": locality_namespace_services,
#     "symbol": symbol_namespace_services,
# }
#
#
# def classify_agas_service(
#         service_name):
#
#     for services_name in agas_services:
#         if service_name in agas_services[services_name]:
#             return services_name
#
#     assert False, service_name
#
#
# cache_statistics = [
#     "entries",
#     "evictions",
#     "hits",
#     "insertions",
#     "misses",
# ]
#
#
# full_cache_statistics = [
#     "erase_entry",
#     "get_entry",
#     "insert_entry",
#     "update_entry",
# ]
#
#
# agas_statistics = {
#     "cache": cache_statistics,
#     "full_cache": full_cache_statistics,
# }
#
#
# def classify_agas_cache_statistic(
#         statistic_name):
#
#     for statistics_name in agas_statistics:
#         if statistic_name in agas_statistics[statistics_name]:
#             return statistics_name
#
#     assert False, statistic_name
#
#
# def classify_performance_counter(
#         counter):
#
#     group_name = counter.type
#
#     # By default:
#     # - Group counter per /object{instance.parent}/name
#     # - Label counter by instance.instance
#     # This implies that for each counter name a plot is made per locality,
#     # and that each counter is named after the instance within the locality
#     group_name = "{}{{{}}}/{}".format(
#         counter.object, counter.instance.parent, counter.name)
#     counter_label = counter.instance.instance
#
#     # Exceptions to the default grouping rule
#     if counter.object == "agas":
#         if counter.name.startswith("count/cache"):
#             # - Group per instance
#             # - Group per statistic kind
#             name, _, statistic_name = counter.name.rpartition("/")
#             statistic_kind = classify_agas_cache_statistic(statistic_name)
#             group_name = "agas{{{}}}/{}/<{}>".format(
#                 counter.instance.full, name, statistic_kind)
#             counter_label = statistic_name
#         elif counter.name.startswith("time/cache"):
#             name, _, statistic = counter.name.rpartition("/")
#             group_name = "agas{{{}}}/{}".format(counter.instance.full, name)
#             counter_label = statistic
#         elif counter.name.startswith("count"):
#             # - Group per instance
#             # - Group per AGAS service kind
#             agas_service_name = counter.name.split("/")[1]
#             agas_services_name = classify_agas_service(agas_service_name)
#             group_name = "agas{{{}}}/count/<{}>".format(
#                 counter.instance.full, agas_services_name)
#             counter_label = agas_service_name
#
#     return group_name, counter_label
#
#
# def group_performance_counters(
#         counter_names):
#     """
#     Given the names of the counters produced, create groups that are
#     useful to combine in plots. Also add some information to improve
#     the plots, like limits in y.
#
#     This function contains counter-specific knowledge.
#     """
#     # A dict with per group a high level name, also used as key, and a
#     # group instance containing all information about the group
#     groups = collections.OrderedDict()
#
#     # Counters are grouped according to the object. Within this group,
#     # counters are also grouped.
#
#     for counter_name in counter_names:
#         # Classify counter and add to group. Create group if not
#         # existant yet.
#         counter = parse_performance_counter_name(counter_name)
#
#         # Counters related to the same object
#         object_groups = groups.setdefault(
#             counter.object, collections.OrderedDict())
#
#         group_name, counter_label = classify_performance_counter(counter)
#         counter_group = object_groups.setdefault(
#             group_name, PerformanceCounterGroup(group_name))
#         counter.label = counter_label
#         counter_group.counters.append(counter)
#
#     for names, object_groups in groups.items():
#         for name, group in object_groups.items():
#             if "idle-rate" in name:
#                 group.ylim = (0, 100)
#
#     return groups
#
#
# def plot_performance_counters(
#         lue_dataset,
#         nr_workers):
#
#     # Read performance counters and plot them, grouped
#     # - Get rid of seaborn stuff
#     # - Try to be faster than before
#     # - Try to be more robust, also when plotting large graphs (subset?)
#
#     property_set_name = "performance_counter_{}".format(nr_workers)
#     property_set = lue_dataset.benchmark.property_sets[property_set_name]
#
#     nr_time_units = property_set.time_domain.clock.nr_units
#     nr_time_points = property_set.time_domain.value[:]
#     assert nr_time_points.shape == (1, 2), nr_time_points.shape
#     assert nr_time_points[0][0] % nr_time_units == 0
#     assert nr_time_points[0][1] % nr_time_units == 0
#     nr_time_points = nr_time_points / nr_time_units
#     time = np.arange(nr_time_points[0][0], nr_time_points[0][1])
#
#     counter_dirname = os.path.split(lue_dataset.pathname)[0]
#
#     plot_pathnames = []
#
#     property_names = property_set.properties.names
#     counter_names = [
#         util.property_name_to_performance_counter_name(name)
#             for name in property_names]
#
#     groups_per_object = group_performance_counters(counter_names)
#
#     for object_name in groups_per_object:
#         # object_name: agas, threads, parcels, parcelqueue, runtime,
#         #     threadqueue, ...
#         counter_groups = groups_per_object[object_name]
#
#         directory_pathname = \
#             os.path.join(counter_dirname, object_name, str(nr_workers))
#
#         if not os.path.isdir(directory_pathname):
#             os.makedirs(directory_pathname)
#
#         for counter_group_name in counter_groups:
#
#             counter_group = counter_groups[counter_group_name]
#             counter_group_basename = \
#                 util.performance_counter_name_to_property_name(
#                     counter_group_name)
#
#             plot_pathname = os.path.join(
#                 directory_pathname, "{}.pdf".format(counter_group_basename))
#             assert not plot_pathname in plot_pathnames
#             plot_pathnames.append(plot_pathname)
#
#             figure, axes = plt.subplots(
#                     nrows=1, ncols=1,
#                     figsize=(8, 6),
#                     squeeze=False, sharex=False
#                 )  # Inches...
#
#             for counter in counter_group.counters:
#                 property_name = \
#                     util.performance_counter_name_to_property_name(counter.full_name)
#                 # Index of one and only time box (0) and of one and only
#                 # object (5)
#                 counter_data = \
#                     property_set.properties[property_name].value[0][5][:]
#
#                 axes[0, 0].plot(
#                     time, counter_data, linewidth=plot.default_linewidth)
#
#             axes[0, 0].set_ylabel(counter_group.name)
#             axes[0, 0].grid()
#             axes[0, 0].set_title(counter_group.name)
#             axes[0, 0].set_xlabel(
#                 "interval (x {} milliseconds)".format(nr_time_units))
#             axes[0, 0].set_ylim(*counter_group.ylim)
#
#             if len(counter_group) <= 10:
#                 axes[0, 0].legend(
#                     labels=[
#                         counter.label for counter in counter_group.counters])
#
#             plt.savefig(plot_pathname, bbox_inches="tight")
#             plt.close(figure)


# def post_process_performance_counters(
#         lue_dataset,
#         experiment):
#
#     nr_workers = lue_dataset.benchmark.measurement.nr_workers.value[:]
#
#     for i in trange(len(nr_workers), desc="plot performance counters"):
#         plot_performance_counters(lue_dataset, nr_workers[i])


def postprocess_results(configuration_data):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function
    """

    configuration = Configuration(configuration_data)
    cluster = configuration.cluster
    benchmark = configuration.benchmark
    result_prefix = configuration.result_prefix
    experiment = configuration.experiment

    lue_dataset = job.open_scalability_lue_dataset(
        result_prefix, cluster, benchmark, experiment, "r"
    )
    cluster, benchmark, experiment = dataset.read_benchmark_settings(
        lue_dataset, Experiment
    )

    performance_counters_available = (
        "performance_counter_1" in lue_dataset.benchmark.property_sets.names
    )

    if not performance_counters_available:
        process.create_dot_graph(
            lue_dataset.pathname,
            experiment.result_pathname(
                result_prefix, cluster.name, benchmark.scenario_name, "graph", "pdf"
            ),
        )

    plot_pathname = experiment.result_pathname(
        result_prefix, cluster.name, benchmark.scenario_name, "plot", "pdf"
    )
    post_process_raw_results(lue_dataset, result_prefix, plot_pathname)

    # TODO revamp
    # if performance_counters_available:
    #     post_process_performance_counters(lue_dataset, experiment)
