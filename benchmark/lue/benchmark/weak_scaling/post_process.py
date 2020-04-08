# -*- encoding: utf8 -*-
from .weak_scaling_experiment import WeakScalingExperiment
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

        serial_duration = duration[0] * nr_workers
        axis.plot(
            nr_workers, serial_duration, linewidth=plot.default_linewidth,
            color=plot.serial_color)

        linear_duration = \
            np.array([duration[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers, linear_duration, linewidth=plot.default_linewidth,
            color=plot.linear_color)

        plot_actual(duration)

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
    plot_relative_efficiency(axes[0][1])
    plot_lups(axes[1][0])
    axes[1, 1].axis("off")

    figure.legend(labels=["linear", "serial", "actual"])

    name = lue_meta_information.name.value[:][0]
    system_name = lue_meta_information.system_name.value[:][0]
    scenario_name = lue_meta_information.scenario_name.value[:][0]
    array_shape_per_worker = lue_meta_information.array_shape.value[0]
    partition_shape = lue_meta_information.partition_shape.value[0]

    figure.suptitle(
        "{}, {}, {}\n"
        "Weak scaling experiment on {} array per worker and {} partitions ({})"
            .format(
                name,
                system_name,
                time_point,
                "x".join([str(extent) for extent in array_shape_per_worker]),
                "x".join([str(extent) for extent in partition_shape]),
                scenario_name,
            )
        )

    plt.savefig(plot_pathname)


def post_process_results(
        results_prefix):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function.
    """
    lue_dataset = job.open_lue_dataset(results_prefix, "r")
    cluster, benchmark, experiment = dataset.read_benchmark_settings(
        lue_dataset, WeakScalingExperiment)

    util.create_dot_graph(
        lue_dataset.pathname,
        experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "graph", "pdf"))
    plot_pathname = experiment.result_pathname(
            cluster.name, benchmark.scenario_name, "plot", "pdf")

    post_process_raw_results(lue_dataset, plot_pathname)
