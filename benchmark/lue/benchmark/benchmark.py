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
        self.scenario_name = json["scenario"] if "scenario" in json else None
        self.count = json["count"]

        self.locality_per = json["locality_per"]
        assert self.locality_per in ["cluster_node", "numa_node"]

        self.worker = worker.Worker(json["worker"], cluster, self.locality_per)
        self.hpx = HPX(json["hpx"]) if "hpx" in json else None

        if self.worker.type == "thread":
            if self.locality_per == "numa_node":
                self.nr_threads_per_locality = cluster.node.package.numa_node.nr_cores
                # self.thread_binding = "scatter"

                assert self.worker.min_nr_cluster_nodes == 1, self.worker.min_nr_cluster_nodes
                assert self.worker.max_nr_cluster_nodes == 1, self.worker.max_nr_cluster_nodes
                assert self.worker.min_nr_numa_nodes == 1, self.worker.min_nr_numa_nodes
                assert self.worker.max_nr_numa_nodes == 1, self.worker.max_nr_numa_nodes
                assert self.worker.min_nr_threads >= 1, self.worker.min_nr_threads
                assert self.worker.max_nr_threads <= self.nr_threads_per_locality, self.nr_threads_per_locality

            elif self.locality_per == "cluster_node":
                self.nr_threads_per_locality = cluster.node.nr_cores
                # self.thread_binding = "scatter"

                # # Bind OS threads to the first processing unit of each core
                # self.thread_binding = \
                #         "thread:0-{}=core:0-{}.pu:0" \
                #     .format(
                #         cluster.node.nr_cores-1,
                #         cluster.node.nr_cores-1)


                assert self.worker.min_nr_cluster_nodes == 1, self.worker.min_nr_cluster_nodes
                assert self.worker.max_nr_cluster_nodes == 1, self.worker.max_nr_cluster_nodes
                assert self.worker.min_nr_numa_nodes == self.worker.min_nr_numa_nodes, self.worker.min_nr_numa_nodes
                assert self.worker.max_nr_numa_nodes == self.worker.max_nr_numa_nodes, self.worker.max_nr_numa_nodes
                assert self.worker.min_nr_threads >= 1, self.worker.min_nr_threads
                assert self.worker.max_nr_threads <= self.nr_threads_per_locality, self.nr_threads_per_locality



    def __str__(self):
        return "Benchmark(count={}, worker={})" \
            .format(
                self.count,
                self.worker
            )
