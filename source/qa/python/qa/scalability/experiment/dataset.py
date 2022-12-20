from .benchmark import Benchmark
from .cluster import Cluster
import lue.data_model as ldm
import numpy as np
import json


def write_benchmark_settings(
        lue_dataset,
        cluster,
        benchmark,
        experiment):

    ldm.assert_is_valid(lue_dataset, fail_on_warning=False)

    phenomenon = lue_dataset.add_phenomenon("benchmark")
    property_set = phenomenon.add_collection_property_set("meta_information")

    cluster_json = json.dumps(cluster.to_json(), sort_keys=False, indent=4)
    cluster_property = property_set.add_property(
        "cluster_settings", dtype=np.dtype(np.unicode_))
    cluster_property.value.expand(1)[:] = np.array([cluster_json])

    benchmark_json = json.dumps(benchmark.to_json(), sort_keys=False, indent=4)
    benchmark_property = property_set.add_property(
        "benchmark_settings", dtype=np.dtype(np.unicode_))
    benchmark_property.value.expand(1)[:] = np.array([benchmark_json])

    experiment_json = json.dumps(experiment.to_json(), sort_keys=False, indent=4)
    experiment_property = property_set.add_property(
        "experiment_settings", dtype=np.dtype(np.unicode_))
    experiment_property.value.expand(1)[:] = np.array([experiment_json])

    ldm.assert_is_valid(lue_dataset, fail_on_warning=False)


def write_script(
        lue_dataset,
        script):

    ldm.assert_is_valid(lue_dataset)

    script_property = lue_dataset.benchmark.meta_information.add_property(
        "script", dtype=np.dtype(np.unicode_))
    script_property.value.expand(1)[:] = np.array([script])

    ldm.assert_is_valid(lue_dataset)


def read_benchmark_settings(
        lue_dataset,
        Experiment):

    ldm.assert_is_valid(lue_dataset, fail_on_warning=False)

    phenomenon = lue_dataset.benchmark
    property_set = phenomenon.meta_information

    cluster = Cluster(
        json.loads(property_set.cluster_settings.value[:][0]))
    benchmark = Benchmark(
        json.loads(property_set.benchmark_settings.value[:][0]), cluster)
    experiment = Experiment(
        json.loads(property_set.experiment_settings.value[:][0]))

    return cluster, benchmark, experiment


def raw_results_already_imported(
        lue_dataset):

    ldm.assert_is_valid(lue_dataset, fail_on_warning=False)

    # Test for something that is being imported
    return \
        "benchmark" in lue_dataset.phenomena and \
        "meta_information" in \
            lue_dataset.benchmark.collection_property_sets and \
        "name" in lue_dataset.benchmark.meta_information.properties
