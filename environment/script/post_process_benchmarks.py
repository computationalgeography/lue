#!/usr/bin/env python
import lue
import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import docopt
import os.path
import sys


usage = """\
Post-process benchmark results

Usage:
    {command} <lue_file>

Options:
    lue_file        Pathname to file containing benchmark results
    -h --help       Show this screen
""".format(
    command = os.path.basename(sys.argv[0]))


def post_process_strong_scaling_benchmarks(
        environment,
        durations):

    data = pd.merge(environment, durations, left_index=True, right_index=True)

    # Group measurements by benchmark and calculate the mean
    durations_by_nr_threads = \
        data[["nr_threads", "duration"]].groupby("nr_threads").mean()
    t1 = durations_by_nr_threads.at[1, "duration"]

    durations_by_nr_threads["ideal_duration"] = \
        t1 / durations_by_nr_threads.index

    # speedup = t1 / tn
    durations_by_nr_threads["relative_speedup"] = \
        t1 / durations_by_nr_threads["duration"]
    durations_by_nr_threads["ideal_relative_speedup"] = \
        durations_by_nr_threads.index

    # efficiency = 100% * speedup / nr_workers
    durations_by_nr_threads["efficiency"] = \
        100 * durations_by_nr_threads["relative_speedup"] / \
        durations_by_nr_threads.index
    durations_by_nr_threads["ideal_efficiency"] = 100

    max_nr_threads = environment["nr_threads"].max()

    figure, axes = plt.subplots(
        nrows=3, ncols=1,
        figsize=(5, 8),  # Inches...
        sharex=True)

    # grid = sns.relplot(x="nr_threads", y="duration", kind="line", data=data,
    #     legend="full", ci="sd", ax=axes[0, 0])

    # duration by nr_threads
    grid = sns.lineplot(
        data=durations_by_nr_threads["ideal_duration"],
        ax=axes[0])
    grid = sns.lineplot(
        x="nr_threads", y="duration", data=data,
        ax=axes[0])
    axes[0].set_ylabel("duration ({})".format("todo"))

    # speedup by nr_threads
    grid = sns.lineplot(
        data=durations_by_nr_threads["ideal_relative_speedup"],
        ax=axes[1])
    grid = sns.lineplot(
        data=durations_by_nr_threads["relative_speedup"],
        ax=axes[1])
    axes[1].set_ylim(0, max_nr_threads + 1)
    axes[1].set_ylabel("relative speedup")

    # efficiency by nr_threads
    grid = sns.lineplot(
        data=durations_by_nr_threads["ideal_efficiency"],
        ax=axes[2])
    grid = sns.lineplot(
        data=durations_by_nr_threads["efficiency"],
        ax=axes[2])
    axes[2].set_ylim(0, 110)
    axes[2].set_ylabel("efficiency (%)")
    axes[2].set_xlabel("number of threads")

    # plt.setp(axes, xlabel="meh")

    plt.savefig("benchmark.pdf")


def post_process_weak_scaling_benchmarks(
        environment,
        durations):

    # TODO
    pass


def post_process_benchmarks(
        lue_pathname):

    lue_dataset = lue.open_dataset(lue_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]
    lue_measurement = lue_benchmark.property_sets["measurement"]

    lue_nr_localities = lue_measurement.properties["nr_localities"]
    lue_nr_threads = lue_measurement.properties["nr_threads"]
    lue_work_size = lue_measurement.properties["work_size"]
    lue_duration = lue_measurement.properties["duration"]

    nr_localities = lue_nr_localities.value[:]
    nr_measurements = len(nr_localities)
    nr_threads = lue_nr_threads.value[:]
    assert(len(nr_threads) == nr_measurements)
    work_size = lue_work_size.value[:]
    assert(len(work_size) == nr_measurements)

    duration = lue_duration.value[:]
    assert(len(duration) == nr_measurements)
    nr_durations = len(duration[0])

    # Set up data frames
    # The (default) index is the index of the benchmark
    environment = pd.DataFrame({
            "nr_localities": nr_localities,
            "nr_threads": nr_threads,
            "work_size": work_size,
        })

    # Per benchmark a series. Each series contains all duration measurements.
    # These series are concatenated in one long series containing the
    # durations for all benchmarks. The index contains the index of
    # the benchmark.
    durations = [
            pd.Series(duration[b], index=nr_durations*[b]) for b in
                range(nr_measurements)
        ]
    durations = pd.DataFrame({
            "duration": pd.concat(durations)
        })


    nr_equal_work_sizes = \
        (environment["work_size"] == environment["work_size"][0]).sum()
    constant_work_size = nr_equal_work_sizes == nr_measurements

    if constant_work_size:
        post_process_strong_scaling_benchmarks(environment, durations)
    else:
        post_process_weak_scaling_benchmarks(environment, durations)


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    lue_pathname = arguments["<lue_file>"]

    post_process_benchmarks(lue_pathname)
