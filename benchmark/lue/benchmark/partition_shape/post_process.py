# -*- encoding: utf8 -*-
from .benchmark import *
from .experiment import *
from ..cluster import *

import lue

import dateutil.relativedelta
import dateutil.parser

import pandas as pd
import matplotlib
# matplotlib.use("PDF")
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

import json
import shlex
import subprocess
import tempfile


def lue_translate():
    return os.path.expandvars("$LUE_OBJECTS/bin/lue_translate")


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
    array_shape = list(experiment.array_shape)

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
                                    "value": ["{}".format(
                                        experiment.program_name)]
                                },
                                {
                                    "name": "system_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(cluster.name)]
                                },
                                {
                                    "name": "command",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(
                                        experiment.command_pathname)]
                                },
                                {
                                    "name": "kind",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format("partition_shape")]
                                },
                                {
                                    "name": "description",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": ["{}".format(
                                        experiment.description)]
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
                                {
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape
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
        epoch=None):

    # Read benchmark JSON
    benchmark_json = json.loads(open(benchmark_pathname).read())

    time_units = "second"
    benchmark_epoch = dateutil.parser.parse(benchmark_json["start"])
    epoch = benchmark_epoch if epoch is None else epoch
    epoch_offset = int((benchmark_epoch - epoch).total_seconds())

    if epoch_offset < 0:
        raise RuntimeError(
            "epoch passed in is later than epoch from benchmark: "
            "{} > {}".format(epoch, benchmark_epoch))

    # Calculate number of seconds sinds the epoch
    time_points = [
        dateutil.parser.parse(timing["start"])
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
                                    "unit": time_units,
                                    "tick_period_count": 1,
                                    "epoch": epoch.isoformat()
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


def execute_command(
        command):

    result = subprocess.call(shlex.split(command))

    if result != 0:
        raise RuntimeError("Error executing {}".format(command))


def import_lue_json(
        lue_json_pathname,
        lue_dataset_pathname):

    command = "{} import --add {} {}".format(
        lue_translate(),
        lue_dataset_pathname,
        lue_json_pathname)
    execute_command(command)


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
    # TODO Handle epoch
    ### The resulting file can be imported into a LUE dataset using the
    ### lue_translate command. With lue_translate multiple JSON files can be
    ### imported. In that case it can be useful to use the --epoch option to
    ### synchronize the epochs from the different JSON files.
    epoch = None

    lue_dataset_pathname = experiment.result_pathname()

    if os.path.exists(lue_dataset_pathname):
        os.remove(lue_dataset_pathname)

    partition_shapes = experiment.partition_shapes()
    metadata_written = False

    for partition_shape in partition_shapes:

        result_pathname = experiment.benchmark_result_pathname(partition_shape)
        assert os.path.exists(result_pathname), result_pathname

        # TODO In case of partition shape experiments, this LUE dataset
        #      must be set up differently than when performing other
        #      experiments!
        if not metadata_written:
            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_meta_to_lue_json(
                    result_pathname, lue_json_file.name, cluster, experiment)
                import_lue_json(lue_json_file.name, lue_dataset_pathname)
            metadata_written = True

        with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
            benchmark_to_lue_json(result_pathname, lue_json_file.name, epoch)
            import_lue_json(lue_json_file.name, lue_dataset_pathname)


def create_dot_graph(
        experiment):
    """
    Create a dot graph of the LUE file containing the experiment results
    """

    lue_dataset_pathname = experiment.result_pathname()
    dot_properties_pathname = os.path.expandvars(
        "$LUE/document/lue_translate/dot_properties.json")
    pdf_graph_pathname = "{}.pdf".format(
        os.path.splitext(os.path.basename(lue_dataset_pathname))[0])

    with tempfile.NamedTemporaryFile(suffix=".dot") as dot_graph_file:
        commands = []
        commands.append(
            "{} export --meta {} {} {}".format(
                lue_translate(),
                dot_properties_pathname,
                lue_dataset_pathname,
                dot_graph_file.name))
        commands.append(
            "dot -Tpdf -o {} {}".format(
                pdf_graph_pathname,
                dot_graph_file.name))

        for command in commands:
            execute_command(command)


def meta_information_dataframe(
        lue_meta_information):

    # TODO: BUG: double delete
    # lue_meta_information.properties["kind"].value[0:0]

    assert \
        lue_meta_information.properties["kind"].value[:] == ["partition_shape"]

    name = lue_meta_information.properties["name"].value[:]
    system_name = lue_meta_information.properties["system_name"].value[:]
    array_shape = lue_meta_information.properties["array_shape"].value[:]

    # Pandas does not support nD array elements. Convert (each) shape
    # to string.
    assert array_shape.dtype == np.uint64
    array_shape = [str(shape) for shape in array_shape]

    meta_information = pd.DataFrame({
            "name": name,
            "system_name": system_name,
            "array_shape": array_shape,
        })

    return meta_information


def measurement_dataframe(
        lue_measurement):

    partition_shape = lue_measurement.properties["partition_shape"].value[:]
    duration = lue_measurement.properties["duration"].value[:]
    assert len(duration) == len(partition_shape)

    duration = pd.DataFrame(duration)
    partition_shape = pd.DataFrame(partition_shape)
    assert (duration.index == partition_shape.index).all()
    measurement = pd.concat([partition_shape, duration], axis=1)

    return measurement


def post_process_raw_results(
        cluster,
        benchmark,
        experiment):
    """
    Create plots and tables from raw benchmark results
    """

    # TODO Pass in parameters for output file naming

    lue_dataset_pathname = experiment.result_pathname()
    lue_dataset = lue.open_dataset(lue_dataset_pathname)
    lue_benchmark = lue_dataset.phenomena["benchmark"]
    lue_meta_information = \
        lue_benchmark.collection_property_sets["meta_information"]
    lue_measurement = lue_benchmark.property_sets["measurement"]

    rank = len(lue_meta_information.properties["array_shape"].value.shape)
    nr_benchmarks = lue_measurement.object_tracker.active_set_index.shape[0]
    count = lue_measurement.properties["duration"].value.shape[1]

    meta_information = meta_information_dataframe(lue_meta_information)
    name = meta_information.name[0]
    system_name = meta_information.system_name[0]

    measurement = measurement_dataframe(lue_measurement)

    # Select shape columns
    partition = measurement.iloc[:, 0:rank]

    partition.insert(0, "partition_size", partition.product(axis=1))
    assert partition["partition_size"].is_unique
    assert partition["partition_size"].is_monotonic_increasing

    # Select count duration columns
    duration = measurement.iloc[:, rank:]

    duration = pd.DataFrame(
        pd.concat([pd.Series(array) for array in duration.values]) # ,
        # index=[count * [v] for v in range(count)]
    )

    duration.set_index(
        np.repeat([v for v in partition["partition_size"]], count),
        inplace=True)

    time_point = "todo"

    figure, axes = plt.subplots(
            nrows=1, ncols=1,
            figsize=(15, 5)
            # sharex=False
        )  # Inches...

    # Duration by partition size

    actual_color = sns.xkcd_rgb["denim blue"]

    sns.lineplot(data=duration, ax=axes, color=actual_color, legend=False)

    axes.set_ylabel(u"duration ± 1 std ({})".format("todo"))
    axes.set_xlabel(u"partition size")

    figure.suptitle(
        "{}\nPartition shape scaling experiment performed at {}, on {}"
            .format(name, time_point, system_name))

    plt.savefig("benchmark.pdf")


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

    benchmark = Benchmark(benchmark_settings_json)
    experiment = Experiment(experiment_settings_json, command_pathname)

    # 1. Import raw results data
    # 2. Add data for plotting
    # 3. Create initial set of plots

    import_raw_results(cluster, benchmark, experiment)

    # TODO Add data underlying the scaling plots to the same dataset
    #      The goal is that publication-ready plots can easily be made
    #      later on
    # add_plot_data()

    create_dot_graph(experiment)
    post_process_raw_results(cluster, benchmark, experiment)
