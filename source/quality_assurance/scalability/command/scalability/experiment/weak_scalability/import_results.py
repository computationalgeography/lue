import json
import os.path
import tempfile

import dateutil.parser
import numpy as np

import lue.data_model as ldm

from ...core import process
from .. import dataset, job, util
from .configuration import Configuration
from .experiment import Experiment


def benchmark_meta_to_lue_json(
    benchmark_pathname, lue_dataset_pathname, cluster, benchmark, experiment
):
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
                                    "value": [experiment.program_name],
                                },
                                {
                                    "name": "system_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [cluster.name],
                                },
                                {
                                    "name": "command",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.command_pathname],
                                },
                                {
                                    "name": "arguments",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.command_arguments],
                                },
                                {
                                    "name": "kind",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.name],
                                },
                                {
                                    "name": "scenario_name",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [benchmark.scenario_name],
                                },
                                {
                                    "name": "description",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [experiment.description],
                                },
                                # {
                                #     "name": "nr_time_steps",
                                #     "shape_per_object": "same_shape",
                                #     "value_variability": "constant",
                                #     "datatype": "uint64",
                                #     "value": [experiment.nr_time_steps]
                                # },
                                {
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape,
                                },
                                {
                                    "name": "partition_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "uint64",
                                    "shape": [len(partition_shape)],
                                    "value": partition_shape,
                                },
                                {
                                    "name": "worker_type",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "constant",
                                    "datatype": "string",
                                    "value": [benchmark.worker.type],
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    # Write results
    open(lue_dataset_pathname, "w").write(
        json.dumps(lue_json, sort_keys=False, indent=4)
    )


def benchmark_to_lue_json(
    nr_workers, benchmark_pathname, lue_json_pathname, epoch, benchmark
):
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
            "epoch passed in is later than epoch from benchmark: {} > {}".format(
                epoch, benchmark_epoch
            )
        )

    # Benchmarks are sorted by benchmark epochs. Keep the information
    # sorted by time as well. Use benchmark epoch instead of individual
    # timings.
    time_points = [epoch_offset]

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
    nr_workers = benchmark_json["environment"]["nr_workers"]

    lue_json = {
        "dataset": {
            "phenomena": [
                {
                    "name": "benchmark",
                    "property_sets": [
                        {
                            "name": "measurement",
                            "description": "Information per benchmark measurement",
                            "object_tracker": {
                                "active_set_index": active_set_idx,
                                "active_object_id": active_object_id,
                            },
                            "time_domain": {
                                "clock": {
                                    "epoch": {
                                        "kind": "common_era",
                                        "origin": epoch.isoformat(),
                                        "calendar": "gregorian",
                                    },
                                    "unit": time_units,
                                    "tick_period_count": 1,
                                },
                                "time_point": time_points,
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
                                    "value": durations,
                                },
                                {
                                    "name": "array_shape",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "shape": [len(array_shape)],
                                    "value": array_shape,
                                },
                                {
                                    "name": "nr_workers",
                                    "shape_per_object": "same_shape",
                                    "value_variability": "variable",
                                    "shape_variability": "constant_shape",
                                    "datatype": "uint64",
                                    "value": [nr_workers],
                                },
                            ],
                        }
                    ],
                }
            ]
        }
    }

    # Write results
    open(lue_json_pathname, "w").write(json.dumps(lue_json, sort_keys=False, indent=4))


def import_raw_results(
    lue_dataset_pathname, result_prefix, cluster, benchmark, experiment
):
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
    # sorted by start location in time.

    # It is possible that the results for a later experiment, with more
    # workers, is stored before an experiment with less workers. This
    # must be taken care of later, during post-processing.
    # -> Results are sorted by time, not by the number of workers!!!

    benchmark_idxs, epoch = util.sort_benchmarks_by_time(
        result_prefix, cluster, benchmark, experiment
    )

    metadata_written = False

    for benchmark_idx in benchmark_idxs:
        nr_workers = benchmark.worker.nr_workers(benchmark_idx)

        result_pathname = experiment.benchmark_result_pathname(
            result_prefix, cluster.name, benchmark.scenario_name, nr_workers, "json"
        )
        assert os.path.exists(result_pathname), result_pathname

        if not metadata_written:
            with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
                benchmark_meta_to_lue_json(
                    result_pathname, lue_json_file.name, cluster, benchmark, experiment
                )
                process.import_lue_json(lue_json_file.name, lue_dataset_pathname)
            metadata_written = True

        with tempfile.NamedTemporaryFile(suffix=".json") as lue_json_file:
            benchmark_to_lue_json(
                nr_workers, result_pathname, lue_json_file.name, epoch, benchmark
            )
            process.import_lue_json(lue_json_file.name, lue_dataset_pathname)

    ldm.assert_is_valid(lue_dataset_pathname)


def write_scalability_results(lue_dataset):
    count = lue_dataset.benchmark.measurement.duration.value.shape[1]

    lue_measurement = lue_dataset.benchmark.measurement
    # lue_meta_information = lue_dataset.benchmark.meta_information

    # Write property-set to dataset containing the scalability information
    # - benchmark
    #     - scaling
    #         - mean_duration
    #         - std_duration
    #         - efficiency
    #         - LUPS

    scaling_property_set = lue_dataset.benchmark.add_property_set(
        "scaling", lue_measurement.time_domain, lue_measurement.object_tracker
    )

    duration = lue_measurement.duration.value[:]
    nr_durations = len(duration)

    nr_workers = lue_measurement.nr_workers.value[:]

    # Results are sorted by time, not by nr_workers. Find index of
    # benchmark with 1 worker.
    t1_idx = np.where(nr_workers == 1)[0][0]
    assert nr_workers[t1_idx] == 1, nr_workers

    nr_workers = nr_workers.reshape(len(nr_workers), 1)

    # Count durations, using one worker
    t1 = duration[t1_idx].astype(np.float64)

    # efficiency = 100% * t1 / tn
    relative_efficiency = 100 * t1 / duration

    relative_efficiency_property = scaling_property_set.add_property(
        "relative_efficiency",
        np.dtype(np.float64),
        shape=(count,),
        value_variability=ldm.ValueVariability.variable,
        description="Relative efficiency: 100% * t1 / nr_workers",
    )
    relative_efficiency_property.value.expand(nr_durations)[:] = relative_efficiency

    # # lups = nr_time_steps * nr_elements / duration
    # # In the case of weak scaling, the nr_elements increases with the
    # # nr_workers. Ideally, LUPS increases linearly with the nr_workers.
    # nr_time_steps = lue_meta_information.nr_time_steps.value[0]
    # array_shape = lue_meta_information.array_shape.value[0]
    # nr_elements = reduce(lambda e1, e2: e1 * e2, array_shape)
    # lups = nr_time_steps * nr_workers * nr_elements / duration

    # lups_property = scaling_property_set.add_property(
    #     "lups", np.dtype(np.float64), shape=(count,),
    #     value_variability=ldm.ValueVariability.variable,
    #     description=
    #         "LUPS: nr_time_steps * nr_elements / duration")
    # lups_property.value.expand(nr_durations)[:] = lups

    if count > 1:
        mean_duration = np.mean(duration, axis=1)
        std_duration = np.std(duration, axis=1)
        assert len(mean_duration) == len(std_duration) == nr_durations

        mean_relative_efficiency = np.mean(relative_efficiency, axis=1)
        std_relative_efficiency = np.std(relative_efficiency, axis=1)

        # mean_lups = np.mean(lups, axis=1)
        # std_lups = np.std(lups, axis=1)

        mean_duration_property = scaling_property_set.add_property(
            "mean_duration",
            np.dtype(np.float64),
            shape=(),
            value_variability=ldm.ValueVariability.variable,
            description="For a number of workers, the mean duration of the {} "
            "experiments took.".format(count),
        )
        mean_duration_property.value.expand(nr_durations)[:] = mean_duration

        std_duration_property = scaling_property_set.add_property(
            "std_duration",
            np.dtype(np.float64),
            shape=(),
            value_variability=ldm.ValueVariability.variable,
            description="For a number of workers, the standard deviation of the "
            "durations the {} experiments took.".format(count),
        )
        std_duration_property.value.expand(nr_durations)[:] = std_duration

        mean_relative_efficiency_property = scaling_property_set.add_property(
            "mean_relative_efficiency",
            np.dtype(np.float64),
            shape=(),
            value_variability=ldm.ValueVariability.variable,
            description="For a number of workers, the mean of the relative "
            "efficiency of the {} experiments.".format(count),
        )
        mean_relative_efficiency_property.value.expand(nr_durations)[:] = (
            mean_relative_efficiency
        )

        std_relative_efficiency_property = scaling_property_set.add_property(
            "std_relative_efficiency",
            np.dtype(np.float64),
            shape=(),
            value_variability=ldm.ValueVariability.variable,
            description="For a number of workers, the standard deviation of the "
            "relative efficiency of the {} experiments.".format(count),
        )
        std_relative_efficiency_property.value.expand(nr_durations)[:] = (
            std_relative_efficiency
        )

        # mean_lups_property = scaling_property_set.add_property(
        #     "mean_lups", np.dtype(np.float64), shape=(),
        #     value_variability=ldm.ValueVariability.variable,
        #     description=
        #         "For a number of workers, the mean of the LUPS of the {} "
        #         "experiments."
        #             .format(count))
        # mean_lups_property.value.expand(nr_durations)[:] = mean_lups

        # std_lups_property = scaling_property_set.add_property(
        #     "std_lups", np.dtype(np.float64), shape=(),
        #     value_variability=ldm.ValueVariability.variable,
        #     description=
        #         "For a number of workers, the standard deviation of the "
        #         "LUPS of the {} experiments."
        #             .format(count))
        # std_lups_property.value.expand(nr_durations)[:] = std_lups


def import_results(configuration_data):
    configuration = Configuration(configuration_data)
    cluster = configuration.cluster
    benchmark = configuration.benchmark
    result_prefix = configuration.result_prefix
    experiment = configuration.experiment

    lue_dataset = job.open_raw_lue_dataset(
        result_prefix, cluster, benchmark, experiment, "r"
    )
    raw_results_already_imported = dataset.raw_results_already_imported(lue_dataset)

    cluster, benchmark, experiment = dataset.read_benchmark_settings(
        lue_dataset, Experiment
    )

    if not raw_results_already_imported:
        lue_dataset_pathname = lue_dataset.pathname
        del lue_dataset
        import_raw_results(
            lue_dataset_pathname, result_prefix, cluster, benchmark, experiment
        )

    if not raw_results_already_imported or not job.scalability_lue_dataset_exists(
        result_prefix, cluster, benchmark, experiment
    ):
        # Copy dataset and write scalability results
        job.copy_raw_to_scalability_lue_dataset(
            result_prefix, cluster, benchmark, experiment
        )
        lue_dataset = job.open_scalability_lue_dataset(
            result_prefix, cluster, benchmark, experiment, "w"
        )
        write_scalability_results(lue_dataset)

        ldm.assert_is_valid(lue_dataset, fail_on_warning=False)
