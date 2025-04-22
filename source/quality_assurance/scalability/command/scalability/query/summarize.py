import json
import typing
from pathlib import Path

import numpy as np

import lue.data_model as ldm

from ..experiment.cluster import Cluster


MS_TO_S = 1 / 1000
MS_TO_MIN = MS_TO_S / 60
MS_TO_H = MS_TO_MIN / 60


def print_message(label: str, value: typing.Any):
    print(f"{label:{20}}: {value}")


def print_total_duration(durations: np.ndarray):
    # Assumes duration is in millisconds
    total_duration_ms = durations.sum()
    total_duration_s = MS_TO_S * total_duration_ms
    total_duration_min = MS_TO_MIN * total_duration_ms
    total_duration_h = MS_TO_H * total_duration_ms

    print_message(
        "total duration",
        f"{total_duration_ms}ms / "
        f"{round(total_duration_s)}s / "
        f"{round(total_duration_min)}min / {total_duration_h:.2f}h",
    )


def scenario_name_to_kind_of_worker(name):
    if name in ["core_numa_node", "core_cluster_node"]:
        result = "core"
    elif name == "numa_node":
        result = "numa_node"
    elif name == "cluster_node":
        result = "cluster_node"
    else:
        result = ""

    assert result, f"Unknown scenario name: {name}"

    return result


def kind_of_worker_to_nr_cores_used(
    nr_workers_used: np.ndarray, kind_of_worker: str, cluster: Cluster
):
    if kind_of_worker == "core":
        # A worker is a single core
        nr_cores_used = nr_workers_used
    elif kind_of_worker == "numa_node":
        # A worker is a single NUMA node
        nr_cores_used = (
            nr_workers_used * cluster.cluster_node.package.numa_node.nr_cores
        )
    elif kind_of_worker == "cluster_node":
        # A worker is a single cluster node
        nr_cores_used = nr_workers_used * cluster.cluster_node.nr_cores
    else:
        nr_cores_used = np.array([-1])

    assert np.all(nr_cores_used > 0), f"unknown kind of worker: {kind_of_worker}"

    return nr_cores_used[:, np.newaxis]


def summarize_partition_shape_experiment(dataset: ldm.Dataset) -> None:
    durations = dataset.benchmark.measurement.duration.value[:]
    cluster_settings = json.loads(
        dataset.benchmark.meta_information.cluster_settings.value[:][0]
    )
    cluster = Cluster(data=cluster_settings)
    scenario_name = dataset.benchmark.meta_information.scenario_name.value[:][0]
    kind_of_worker = scenario_name_to_kind_of_worker(scenario_name)
    nr_workers_used = dataset.benchmark.meta_information.nr_workers.value[:]

    assert len(nr_workers_used) == 1
    nr_runs = durations.shape[0]
    nr_workers_used = np.full(
        shape=(nr_runs,), fill_value=nr_workers_used[0], dtype=nr_workers_used.dtype
    )

    # The number of core hours is the total duration times the number of cores used. The number of cores used
    # is constant while scaling the partition shape.
    nr_cores_used = kind_of_worker_to_nr_cores_used(
        nr_workers_used, kind_of_worker, cluster
    )

    durations_h = MS_TO_H * durations
    nr_core_hours = (nr_cores_used * durations_h).sum()

    print_total_duration(durations)
    print_message("nr cores used", nr_cores_used.T[0])
    print_message("nr core hours", f"{nr_core_hours:.2f}")


def summarize_strong_scalability_experiment(dataset: ldm.Dataset) -> None:
    durations = dataset.benchmark.measurement.duration.value[:]
    cluster_settings = json.loads(
        dataset.benchmark.meta_information.cluster_settings.value[:][0]
    )
    cluster = Cluster(data=cluster_settings)
    scenario_name = dataset.benchmark.meta_information.scenario_name.value[:][0]
    kind_of_worker = scenario_name_to_kind_of_worker(scenario_name)
    nr_workers_used = dataset.benchmark.measurement.nr_workers.value[:]

    # The number of core hours is the total duration times the number of cores used. The number of cores
    # used differs.
    nr_cores_used = kind_of_worker_to_nr_cores_used(
        nr_workers_used, kind_of_worker, cluster
    )

    durations_h = MS_TO_H * durations
    nr_core_hours = (nr_cores_used * durations_h).sum()

    print_total_duration(durations)
    print_message("nr cores used", nr_cores_used.T[0])
    print_message("nr core hours", f"{nr_core_hours:.2f}")


def summarize_weak_scalability_experiment(dataset: ldm.Dataset) -> None:
    durations = dataset.benchmark.measurement.duration.value[:]
    cluster_settings = json.loads(
        dataset.benchmark.meta_information.cluster_settings.value[:][0]
    )
    cluster = Cluster(data=cluster_settings)
    scenario_name = dataset.benchmark.meta_information.scenario_name.value[:][0]
    kind_of_worker = scenario_name_to_kind_of_worker(scenario_name)
    nr_workers_used = dataset.benchmark.measurement.nr_workers.value[:]

    # The number of core hours is the total duration times the number of cores used. The number of cores
    # used differs.
    nr_cores_used = kind_of_worker_to_nr_cores_used(
        nr_workers_used, kind_of_worker, cluster
    )

    durations_h = MS_TO_H * durations
    nr_core_hours = (nr_cores_used * durations_h).sum()

    print_total_duration(durations)
    print_message("nr cores used", nr_cores_used.T[0])
    print_message("nr core hours", f"{nr_core_hours:.2f}")


def summarize_experiment(dataset_path: Path) -> None:
    """
    Summarize the experiment whose results are stored in `pathname`
    """
    dataset = ldm.open_dataset(str(dataset_path), "r")
    kind_of_experiment = dataset.benchmark.meta_information.kind.value[:][0]
    kind_of_worker = dataset.benchmark.meta_information.scenario_name.value[:][0]
    # nr_workers = dataset.benchmark.meta_information.nr_workers.value[:][0]
    system_name = dataset.benchmark.meta_information.system_name.value[:][0]

    print_message("dataset path", dataset_path.absolute())
    print_message("system name", system_name)
    print_message("kind of experiment", kind_of_experiment)
    print_message("kind of worker", kind_of_worker)
    # print_message("number or workers", nr_workers)

    if kind_of_experiment == "partition_shape":
        summarize_partition_shape_experiment(dataset)
    elif kind_of_experiment == "strong_scalability":
        summarize_strong_scalability_experiment(dataset)
    elif kind_of_experiment == "weak_scalability":
        summarize_weak_scalability_experiment(dataset)
