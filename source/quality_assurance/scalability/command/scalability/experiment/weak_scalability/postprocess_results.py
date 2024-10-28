import dateutil.parser
import matplotlib.pyplot as plt
import numpy as np

import lue.data_model as ldm

from ...core import math, plot, process
from .. import dataset, job
from .configuration import Configuration
from .experiment import Experiment


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

        serial_duration = duration[0] * nr_workers
        axis.plot(
            nr_workers,
            serial_duration,
            linewidth=plot.default_linewidth,
            color=plot.serial_color,
        )

        linear_duration = np.array([duration[0] for n in range(len(nr_workers))])
        axis.plot(
            nr_workers,
            linear_duration,
            linewidth=plot.default_linewidth,
            color=plot.linear_color,
        )

        plot_actual(duration)

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

    nr_plot_rows = 1
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
    plot_relative_efficiency(axes[0][1])
    # plot_lups(axes[1][0])
    # axes[1, 1].axis("off")

    figure.legend(labels=["serial", "linear", "actual"])

    name = lue_meta_information.name.value[:][0]
    system_name = lue_meta_information.system_name.value[:][0]
    scenario_name = lue_meta_information.scenario_name.value[:][0]
    array_shape_per_worker = lue_meta_information.array_shape.value[0]
    partition_shape = lue_meta_information.partition_shape.value[0]

    figure.suptitle(
        "{}, {}, {}\n"
        "Weak scalability experiment on {} array per worker and {} partitions ({})".format(
            name,
            system_name,
            time_point,
            "x".join([str(extent) for extent in array_shape_per_worker]),
            "x".join([str(extent) for extent in partition_shape]),
            scenario_name,
        )
    )

    plt.savefig(plot_pathname, bbox_inches="tight")


def postprocess_results(configuration_data):
    """
    Post-process the results of executing the benchmark script generated
    by the generate_script function.
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
