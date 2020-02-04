from . import worker


class HPX(object):

    def __init__(self, json):
        self.performance_counters = json["performance_counters"] \
            if "performance_counters" in json else None

        # print-counter-destination={destination}
        #     {
        #         "print-counter-destination": "counter.csv"
        #     },


class Benchmark(object):

    def __init__(self, json, cluster):
        """
        Class for storing information about the configuration of the
        scaling experiment benchmarks

        count: How many times a benchmark needs to be repeated
        locality_per: Whether to launch a process per cluster node or
            NUMA node
        worker: Information about the workers to be used
        """
        self.scenario_name = json["scenario"] \
            if "scenario" in json else "default"
        self.count = json["count"]

        self.locality_per = json["locality_per"]
        assert self.locality_per in ["cluster_node", "numa_node"]

        self.worker = worker.Worker(json["worker"], cluster, self.locality_per)
        self.hpx = HPX(json["hpx"]) if "hpx" in json else None

        if self.worker.type == "thread":
            if self.locality_per == "numa_node":
                self.nr_logical_cores_per_locality = \
                    cluster.cluster_node.package.numa_node.nr_threads
                self.nr_physical_cores_per_locality = \
                    cluster.cluster_node.package.numa_node.nr_cores

                assert self.worker.min_nr_cluster_nodes == 1, \
                    self.worker.min_nr_cluster_nodes
                assert self.worker.max_nr_cluster_nodes == 1, \
                    self.worker.max_nr_cluster_nodes
                assert self.worker.min_nr_numa_nodes == 1, \
                    self.worker.min_nr_numa_nodes
                assert self.worker.max_nr_numa_nodes == 1, \
                    self.worker.max_nr_numa_nodes
                assert self.worker.min_nr_threads >= 1, \
                    self.worker.min_nr_threads
                assert self.worker.max_nr_threads <= \
                        self.nr_physical_cores_per_locality, \
                    self.worker.max_nr_threads

            elif self.locality_per == "cluster_node":
                self.nr_logical_cores_per_locality = cluster.cluster_node.nr_threads
                self.nr_physical_cores_per_locality = cluster.cluster_node.nr_cores

                assert self.worker.min_nr_cluster_nodes == 1, \
                    self.worker.min_nr_cluster_nodes
                assert self.worker.max_nr_cluster_nodes == 1, \
                    self.worker.max_nr_cluster_nodes
                assert self.worker.min_nr_numa_nodes == \
                        self.worker.min_nr_numa_nodes, \
                    self.worker.min_nr_numa_nodes
                assert self.worker.max_nr_numa_nodes == \
                        self.worker.max_nr_numa_nodes, \
                    self.worker.max_nr_numa_nodes
                assert self.worker.min_nr_threads >= 1, \
                    self.worker.min_nr_threads
                assert self.worker.max_nr_threads <= \
                        self.nr_physical_cores_per_locality, \
                    self.worker.max_nr_threads

        elif self.worker.type == "numa_node":

            assert self.locality_per == "numa_node"

            self.nr_logical_cores_per_locality = \
                cluster.cluster_node.package.numa_node.nr_threads
            self.nr_physical_cores_per_locality = \
                cluster.cluster_node.package.numa_node.nr_cores

            assert self.worker.min_nr_cluster_nodes == 1, \
                self.worker.min_nr_cluster_nodes
            assert self.worker.max_nr_cluster_nodes == 1, \
                self.worker.max_nr_cluster_nodes
            assert self.worker.min_nr_numa_nodes >= 1, \
                self.worker.min_nr_numa_nodes
            assert self.worker.max_nr_numa_nodes <= \
                    cluster.cluster_node.nr_numa_nodes, \
                self.worker.min_nr_numa_nodes
            assert self.worker.min_nr_threads == \
                    self.nr_physical_cores_per_locality, \
                self.worker.min_nr_threads
            assert self.worker.max_nr_threads == \
                    self.nr_physical_cores_per_locality, \
                self.worker.max_nr_threads

        elif self.worker.type == "cluster_node":

            assert self.locality_per == "numa_node"

            self.nr_logical_cores_per_locality = \
                cluster.cluster_node.package.numa_node.nr_threads
            self.nr_physical_cores_per_locality = \
                cluster.cluster_node.package.numa_node.nr_cores

            assert self.worker.min_nr_cluster_nodes >= 1, \
                self.worker.min_nr_cluster_nodes
            assert self.worker.max_nr_cluster_nodes <= \
                    cluster.nr_cluster_nodes, self.worker.max_nr_cluster_nodes
            assert self.worker.min_nr_numa_nodes == \
                    cluster.cluster_node.nr_numa_nodes, self.worker.min_nr_numa_nodes
            assert self.worker.max_nr_numa_nodes == \
                    cluster.cluster_node.nr_numa_nodes, self.worker.max_nr_numa_nodes
            assert self.worker.min_nr_threads == \
                    self.nr_physical_cores_per_locality, \
                self.worker.min_nr_threads
            assert self.worker.max_nr_threads == \
                    self.nr_physical_cores_per_locality, \
                self.nr_physical_cores_per_locality

    def __str__(self):
        return "Benchmark(count={}, worker={})" \
            .format(
                self.count,
                self.worker
            )
