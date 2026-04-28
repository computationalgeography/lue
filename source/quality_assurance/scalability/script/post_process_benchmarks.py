#!/usr/bin/env python
# -*- encoding: utf8 -*-
# TODO Use the lue dataset for obtaining information, instead of the json files
import matplotlib

import lue


# matplotlib.use("PDF")
matplotlib.use("Agg")
import os.path
import sys

import docopt
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


usage = """\
Post-process benchmark results

Usage:
    {command} <lue_file>

Options:
    lue_file        Pathname to file containing benchmark results
    -h --help       Show this screen
""".format(command=os.path.basename(sys.argv[0]))


def post_process_strong_scaling_benchmarks(
    name, time_point, system_name, environment, durations
):
    # TODO
    #     - Add units to y-axis duration plot
    #     - Add subtitle
    #         time point of benchmark
    #     - Add work-span stuff

    data = pd.merge(environment, durations, left_index=True, right_index=True)

    # Either the benchmarks scales over threads, or over localities
    scale_over_threads = data["nr_threads"].min() != data["nr_threads"].max()
    scale_over_localities = data["nr_localities"].min() != data["nr_localities"].max()
    assert scale_over_threads != scale_over_localities  # xor
    group_by_column = "nr_threads" if scale_over_threads else "nr_localities"

    # Group measurements by benchmark and calculate the mean
    aggregated_durations = (
        data[[group_by_column, "duration"]].groupby([group_by_column]).mean()
    )

    # t1 = aggregated_durations.at[1, "duration"]
    t1 = aggregated_durations.iat[0, 0]
    nr_workers = aggregated_durations.index.get_level_values(group_by_column)

    # Best case: duration scales perfect with the number of threads
    # 100% parallel code, but without parallelization overhead
    assert nr_workers[0] == 1
    aggregated_durations["linear_duration"] = t1 / nr_workers

    # Worst case: duration does not scale with the number of threads
    # 100% serial code, but without parallelization overhead
    aggregated_durations["serial_duration"] = t1

    # speedup = t1 / tn
    aggregated_durations["relative_speedup"] = t1 / aggregated_durations["duration"]
    aggregated_durations["linear_relative_speedup"] = (
        t1 / aggregated_durations["linear_duration"]
    )
    aggregated_durations["serial_relative_speedup"] = (
        t1 / aggregated_durations["serial_duration"]
    )

    # efficiency = 100% * speedup / nr_workers
    aggregated_durations["efficiency"] = (
        100 * aggregated_durations["relative_speedup"] / nr_workers
    )
    aggregated_durations["linear_efficiency"] = (
        100 * aggregated_durations["linear_relative_speedup"] / nr_workers
    )
    aggregated_durations["serial_efficiency"] = (
        100 * aggregated_durations["serial_relative_speedup"] / nr_workers
    )

    max_nr_workers = nr_workers.max()

    figure, axes = plt.subplots(
        nrows=1,
        ncols=3,
        figsize=(15, 5),
        # sharex=False
    )  # Inches...

    # grid = sns.relplot(x="nr_threads", y="duration", kind="line", data=data,
    #     legend="full", ci="sd", ax=axes[0, 0])

    # https://xkcd.com/color/rgb/
    serial_color = sns.xkcd_rgb["pale red"]
    linear_color = sns.xkcd_rgb["medium green"]
    actual_color = sns.xkcd_rgb["denim blue"]

    # duration by nr_threads
    sns.lineplot(
        data=aggregated_durations["linear_duration"], ax=axes[0], color=linear_color
    )
    sns.lineplot(
        data=aggregated_durations["serial_duration"], ax=axes[0], color=serial_color
    )
    sns.lineplot(
        x=group_by_column, y="duration", data=data, ax=axes[0], color=actual_color
    )
    axes[0].set_ylabel("duration ± 1 std ({})".format("todo"))
    axes[0].set_xlabel(group_by_column)

    # speedup by nr_threads
    sns.lineplot(
        data=aggregated_durations["linear_relative_speedup"],
        ax=axes[1],
        color=linear_color,
    )
    sns.lineplot(
        data=aggregated_durations["serial_relative_speedup"],
        ax=axes[1],
        color=serial_color,
    )
    sns.lineplot(
        data=aggregated_durations["relative_speedup"], ax=axes[1], color=actual_color
    )
    axes[1].set_ylim(0, max_nr_workers + 1)
    axes[1].set_ylabel("relative speedup (-)")
    axes[1].set_xlabel(group_by_column)

    # efficiency by nr_threads
    sns.lineplot(
        data=aggregated_durations["linear_efficiency"], ax=axes[2], color=linear_color
    )
    sns.lineplot(
        data=aggregated_durations["serial_efficiency"], ax=axes[2], color=serial_color
    )
    sns.lineplot(
        data=aggregated_durations["efficiency"], ax=axes[2], color=actual_color
    )
    axes[2].set_ylim(0, 110)
    axes[2].set_ylabel("efficiency (%)")
    axes[2].set_xlabel(group_by_column)

    figure.legend(labels=["linear", "serial", "actual"])

    # plt.setp(axes, xlabel="meh")
    figure.suptitle(
        "{}\nStrong scaling experiment performed at {}, on {}".format(
            name, time_point, system_name
        )
    )

    plt.savefig("benchmark.pdf")


def post_process_weak_scaling_benchmarks(
    name, time_point, system_name, environment, durations
):
    # TODO
    pass


def post_process_benchmarks(lue_pathname):
    lue_dataset = lue.open_dataset(lue_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]

    lue_meta_information = lue_benchmark.collection_property_sets["meta_information"]
    lue_name = lue_meta_information.properties["name"]
    lue_system_name = lue_meta_information.properties["system_name"]

    benchmark_name = lue_name.value[:]
    assert len(benchmark_name) == 1
    benchmark_name = benchmark_name[0]

    time_point = "todo"

    system_name = lue_system_name.value[:]
    assert len(system_name) == 1
    system_name = system_name[0]

    lue_measurement = lue_benchmark.property_sets["measurement"]
    lue_nr_localities = lue_measurement.properties["nr_localities"]
    lue_nr_threads = lue_measurement.properties["nr_threads"]
    lue_work_size = lue_measurement.properties["work_size"]
    lue_duration = lue_measurement.properties["duration"]

    nr_localities = lue_nr_localities.value[:]
    nr_measurements = len(nr_localities)
    nr_threads = lue_nr_threads.value[:]
    assert len(nr_threads) == nr_measurements
    work_size = lue_work_size.value[:]
    assert len(work_size) == nr_measurements

    duration = lue_duration.value[:]
    assert len(duration) == nr_measurements
    nr_durations = len(duration[0])

    # Set up data frames
    # The (default) index is the index of the benchmark
    environment = pd.DataFrame(
        {
            "nr_localities": nr_localities,
            "nr_threads": nr_threads,
            "work_size": work_size,
        }
    )

    # Per benchmark a series. Each series contains all duration measurements.
    # These series are concatenated in one long series containing the
    # durations for all benchmarks. The index contains the index of
    # the benchmark.
    durations = [
        pd.Series(duration[b], index=nr_durations * [b]) for b in range(nr_measurements)
    ]
    durations = pd.DataFrame({"duration": pd.concat(durations)})

    nr_equal_work_sizes = (
        environment["work_size"] == environment["work_size"][0]
    ).sum()
    constant_work_size = nr_equal_work_sizes == nr_measurements

    if constant_work_size:
        post_process_strong_scaling_benchmarks(
            benchmark_name, time_point, system_name, environment, durations
        )
    else:
        post_process_weak_scaling_benchmarks(
            benchmark_name, time_point, system_name, environment, durations
        )


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    lue_pathname = arguments["<lue_file>"]

    post_process_benchmarks(lue_pathname)
