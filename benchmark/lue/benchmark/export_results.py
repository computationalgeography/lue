import lue.data_model as ldm
import numpy as np
import csv


def export_partition_shape_results(
        lue_dataset,
        csv_writer):

    # Assert that the number of array shapes for which experiments where
    # performed is 1
    lue_array = lue_dataset.array.array
    assert lue_array.shape.value.nr_arrays == 1

    # For each array shape for which experiments where performed
    lue_measurement = lue_dataset.benchmark.measurement

    array_shapes = lue_measurement.array_shape.value[:]
    assert np.all(array_shapes == array_shapes[0])

    count = lue_measurement.duration.value.array_shape[:][0]

    lue_partition = lue_dataset.partition.partition

    partition_shape = lue_measurement.partition_shape.value[:]
    nr_partitions = lue_measurement.nr_partitions.value[:,-1]
    assert len(partition_shape) == len(nr_partitions)

    if count == 1:
        assert False, "Implement!"
    else:
        # Write the folowing columns:
        # - partition_shape
        # - nr_partitions
        # - {mean,std}_duration
        csv_writer.writerow([
                    "partition_shape",
                    "nr_partitions",
                    "mean_duration",
                    "std_duration",
                ])

        mean_duration = \
            lue_partition.properties["mean_duration_{}".format(0)].value[:]
        std_duration = \
            lue_partition.properties["std_duration_{}".format(0)].value[:]

        for n in range(len(partition_shape)):
            csv_writer.writerow([
                    "{},{}".format(*partition_shape[n]),
                    nr_partitions[n],
                    mean_duration[n],
                    std_duration[n],
                ])


def export_strong_scaling_results(
        lue_dataset,
        csv_writer):

    lue_measurement = lue_dataset.benchmark.measurement
    count = lue_measurement.duration.value.array_shape[:][0]
    nr_workers = lue_measurement.nr_workers.value[:]
    sort_idxs = np.argsort(nr_workers)
    nr_workers = nr_workers[sort_idxs]

    if count == 1:
        # Write the folowing columns:
        # - nr_workers
        # - relative_speed_up
        # - relative_efficiency
        # - lups
        csv_writer.writerow([
                    "nr_workers",
                    "relative_speed_up",
                    "relative_efficiency",
                    "lups",
                ])

        lue_scaling = lue_dataset.benchmark.scaling
        relative_speed_up = lue_scaling.relative_speed_up.value[:][sort_idxs]
        relative_efficiency = lue_scaling.relative_efficiency.value[:][sort_idxs]
        lups = lue_scaling.lups.value[:][sort_idxs]

        for n in range(len(nr_workers)):
            csv_writer.writerow([
                    nr_workers[n],
                    relative_speed_up[n],
                    relative_efficiency[n],
                    lups[n],
                ])
    else:
        # Write the folowing columns:
        # - nr_workers
        # - {mean,std}_duration
        # - {mean,std}_relative_efficiency
        # - {mean,std}_lups
        csv_writer.writerow([
                    "nr_workers",
                    "mean_duration",
                    "std_duration",
                    "mean_relative_efficiency",
                    "std_relative_efficiency",
                    "mean_lups",
                    "std_lups",
                ])

        lue_scaling = lue_dataset.benchmark.scaling
        mean_duration = lue_scaling.mean_duration.value[:][sort_idxs]
        std_duration = lue_scaling.std_duration.value[:][sort_idxs]
        mean_relative_efficiency = lue_scaling.mean_relative_efficiency.value[:][sort_idxs]
        std_relative_efficiency = lue_scaling.std_relative_efficiency.value[:][sort_idxs]
        mean_lups = lue_scaling.mean_lups.value[:][sort_idxs]
        std_lups = lue_scaling.std_lups.value[:][sort_idxs]

        for n in range(len(nr_workers)):
            csv_writer.writerow([
                    nr_workers[n],
                    mean_duration[n],
                    std_duration[n],
                    mean_relative_efficiency[n],
                    std_relative_efficiency[n],
                    mean_lups[n],
                    std_lups[n],
                ])


def export_weak_scaling_results(
        lue_dataset,
        csv_writer):

    lue_measurement = lue_dataset.benchmark.measurement
    count = lue_measurement.duration.value.array_shape[:][0]
    nr_workers = lue_measurement.nr_workers.value[:]
    sort_idxs = np.argsort(nr_workers)
    nr_workers = nr_workers[sort_idxs]

    if count == 1:
        # Write the folowing columns:
        # - nr_workers
        # - duration
        # - relative_efficiency
        # - lups
        assert False, "Implement!"
    else:
        # Write the folowing columns:
        # - nr_workers
        # - {mean,std}_duration
        # - {mean,std}_relative_efficiency
        # - {mean,std}_lups
        csv_writer.writerow([
                    "nr_workers",
                    "mean_duration",
                    "std_duration",
                    "mean_relative_efficiency",
                    "std_relative_efficiency",
                    "mean_lups",
                    "std_lups",
                ])

        lue_scaling = lue_dataset.benchmark.scaling
        mean_duration = lue_scaling.mean_duration.value[:][sort_idxs]
        std_duration = lue_scaling.std_duration.value[:][sort_idxs]
        mean_relative_efficiency = lue_scaling.mean_relative_efficiency.value[:][sort_idxs]
        std_relative_efficiency = lue_scaling.std_relative_efficiency.value[:][sort_idxs]
        mean_lups = lue_scaling.mean_lups.value[:][sort_idxs]
        std_lups = lue_scaling.std_lups.value[:][sort_idxs]

        for n in range(len(nr_workers)):
            csv_writer.writerow([
                    nr_workers[n],
                    mean_duration[n],
                    std_duration[n],
                    mean_relative_efficiency[n],
                    std_relative_efficiency[n],
                    mean_lups[n],
                    std_lups[n],
                ])


def export_results(
        lue_dataset_pathname,
        csv_file_pathname):

    lue_dataset = ldm.open_dataset(lue_dataset_pathname, "r")
    kind = lue_dataset.benchmark.meta_information.kind.value[:][0]

    with open(csv_file_pathname, "w") as csv_file:
        csv_writer = csv.writer(csv_file)

        export_by_kind = {
                "partition_shape": export_partition_shape_results,
                "strong_scaling": export_strong_scaling_results,
                "weak_scaling": export_weak_scaling_results,
            }

        export_by_kind[kind](lue_dataset, csv_writer)
