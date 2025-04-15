import csv
from pathlib import Path

import numpy as np

import lue.data_model as ldm


def export_duration_partition_shape(dataset: ldm.Dataset, csv_writer) -> None:
    # Assert that the number of array shapes for which experiments where performed is 1
    lue_array = dataset.array.array
    assert lue_array.shape.value.nr_arrays == 1

    # For each array shape for which experiments where performed
    measurement_property_set = dataset.benchmark.measurement

    array_shapes = measurement_property_set.array_shape.value[:]
    assert np.all(array_shapes == array_shapes[0])

    count = measurement_property_set.duration.value.array_shape[:][0]

    partition_property_set = dataset.partition.partition

    partition_shape = measurement_property_set.partition_shape.value[:]

    if count == 1:
        csv_writer.writerow(
            [
                "partition_shape",
                "partition_size",
                "duration",
            ]
        )

        array_idx = 0
        duration = partition_property_set.properties[f"duration_{array_idx}"].value[:]

        for shape_idx, shape in enumerate(partition_shape):
            csv_writer.writerow(
                [
                    "{},{}".format(*shape),
                    np.prod(shape),
                    duration[shape_idx][0],
                ]
            )
    else:
        csv_writer.writerow(
            [
                "partition_shape",
                "partition_size",
                "mean_duration",
                "std_duration",
            ]
        )

        array_idx = 0
        mean_duration = partition_property_set.properties[
            f"mean_duration_{array_idx}"
        ].value[:]
        std_duration = partition_property_set.properties[
            f"std_duration_{array_idx}"
        ].value[:]

        for shape_idx, shape in enumerate(partition_shape):
            csv_writer.writerow(
                [
                    "{},{}".format(*shape),
                    np.prod(shape),
                    mean_duration[shape_idx],
                    std_duration[shape_idx],
                ]
            )


def export_duration_strong_scalability(dataset: ldm.Dataset, csv_writer) -> None:
    measurement_property_set = dataset.benchmark.measurement
    count = measurement_property_set.duration.value.array_shape[:][0]
    nr_workers = measurement_property_set.nr_workers.value[:]
    sort_idxs = np.argsort(nr_workers)
    nr_workers = nr_workers[sort_idxs]

    if count == 1:
        csv_writer.writerow(
            [
                "nr_workers",
                "duration",
                "relative_speed_up",
                "relative_efficiency",
            ]
        )

        lue_scaling = dataset.benchmark.scaling
        duration = measurement_property_set.duration.value[:][sort_idxs]
        relative_speed_up = lue_scaling.relative_speed_up.value[:][sort_idxs]
        relative_efficiency = lue_scaling.relative_efficiency.value[:][sort_idxs]

        for idx, nr_workers_ in enumerate(nr_workers):
            csv_writer.writerow(
                [
                    nr_workers_,
                    duration[idx][0],
                    relative_speed_up[idx][0],
                    relative_efficiency[idx][0],
                ]
            )
    else:
        csv_writer.writerow(
            [
                "nr_workers",
                "mean_duration",
                "std_duration",
                "mean_relative_efficiency",
                "std_relative_efficiency",
            ]
        )

        lue_scaling = dataset.benchmark.scaling
        mean_duration = lue_scaling.mean_duration.value[:][sort_idxs]
        std_duration = lue_scaling.std_duration.value[:][sort_idxs]
        mean_relative_efficiency = lue_scaling.mean_relative_efficiency.value[:][
            sort_idxs
        ]
        std_relative_efficiency = lue_scaling.std_relative_efficiency.value[:][
            sort_idxs
        ]

        for idx, nr_workers_ in enumerate(nr_workers):
            csv_writer.writerow(
                [
                    nr_workers_,
                    mean_duration[idx],
                    std_duration[idx],
                    mean_relative_efficiency[idx],
                    std_relative_efficiency[idx],
                ]
            )


def export_duration_weak_scalability(dataset: ldm.Dataset, csv_writer) -> None:
    measurement_property_set = dataset.benchmark.measurement
    count = measurement_property_set.duration.value.array_shape[:][0]
    nr_workers = measurement_property_set.nr_workers.value[:]
    sort_idxs = np.argsort(nr_workers)
    nr_workers = nr_workers[sort_idxs]

    if count == 1:
        csv_writer.writerow(
            [
                "nr_workers",
                "duration",
                "relative_efficiency",
            ]
        )

        lue_scaling = dataset.benchmark.scaling
        duration = measurement_property_set.duration.value[:]
        relative_efficiency = lue_scaling.relative_efficiency.value[:][sort_idxs]

        for idx, nr_workers_ in enumerate(nr_workers):
            csv_writer.writerow(
                [
                    nr_workers_,
                    duration[idx][0],
                    relative_efficiency[idx][0],
                ]
            )
    else:
        csv_writer.writerow(
            [
                "nr_workers",
                "mean_duration",
                "std_duration",
                "mean_relative_efficiency",
                "std_relative_efficiency",
            ]
        )

        lue_scaling = dataset.benchmark.scaling
        mean_duration = lue_scaling.mean_duration.value[:][sort_idxs]
        std_duration = lue_scaling.std_duration.value[:][sort_idxs]
        mean_relative_efficiency = lue_scaling.mean_relative_efficiency.value[:][
            sort_idxs
        ]
        std_relative_efficiency = lue_scaling.std_relative_efficiency.value[:][
            sort_idxs
        ]

        for idx, nr_workers_ in enumerate(nr_workers):
            csv_writer.writerow(
                [
                    nr_workers_,
                    mean_duration[idx],
                    std_duration[idx],
                    mean_relative_efficiency[idx],
                    std_relative_efficiency[idx],
                ]
            )


def export_duration(dataset_path: Path, csv_path: Path) -> None:
    """
    Export the duration property stored in `pathname`
    """
    dataset = ldm.open_dataset(str(dataset_path), "r")
    kind_of_experiment = dataset.benchmark.meta_information.kind.value[:][0]

    with open(csv_path, "w", encoding="utf-8") as csv_file:
        csv_writer = csv.writer(csv_file)

        export_by_kind = {
            "partition_shape": export_duration_partition_shape,
            "strong_scalability": export_duration_strong_scalability,
            "weak_scalability": export_duration_weak_scalability,
        }

        export_by_kind[kind_of_experiment](dataset, csv_writer)
