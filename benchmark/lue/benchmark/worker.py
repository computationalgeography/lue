from . import pool


class Worker(object):

    def __init__(self, data, cluster, locality_per):
        """
        Class for storing information about the workers to be used
        in scaling experiments

        Three types of workers can be added during an experiment:
        - thread: used in scaling experiments on
            - a single cluster node
            - a single NUMA node within a cluster node
        - numa_node: used in scaling experiments on a cluster node
            containing NUMA nodes
        - cluster_node: used in scaling experiments on a cluster of nodes
        """

        self.type = data["type"]
        assert self.type in ["thread", "numa_node", "cluster_node"], self.type

        self.pool = pool.Pool(data["pool"])

        if self.type == "thread":
            self.min_nr_cluster_nodes = 1
            self.max_nr_cluster_nodes = 1
            self.min_nr_threads = self.pool.min_size
            self.max_nr_threads = self.pool.max_size

            if locality_per == "cluster_node":
                self.min_nr_numa_nodes = cluster.cluster_node.nr_numa_nodes
                self.max_nr_numa_nodes = cluster.cluster_node.nr_numa_nodes
            elif locality_per == "numa_node":
                self.min_nr_numa_nodes = 1
                self.max_nr_numa_nodes = 1

        elif self.type == "numa_node":
            assert locality_per == "numa_node"

            self.min_nr_cluster_nodes = 1
            self.max_nr_cluster_nodes = 1
            self.min_nr_numa_nodes = self.pool.min_size
            self.max_nr_numa_nodes = self.pool.max_size
            self.min_nr_threads = cluster.cluster_node.package.numa_node.nr_cores
            self.max_nr_threads = cluster.cluster_node.package.numa_node.nr_cores

        elif self.type == "cluster_node":
            assert locality_per == "numa_node"

            self.min_nr_cluster_nodes = self.pool.min_size
            self.max_nr_cluster_nodes = self.pool.max_size
            self.min_nr_numa_nodes = cluster.cluster_node.nr_numa_nodes
            self.max_nr_numa_nodes = cluster.cluster_node.nr_numa_nodes
            self.min_nr_threads = cluster.cluster_node.package.numa_node.nr_cores
            self.max_nr_threads = self.min_nr_threads

        assert 1 <= self.min_nr_cluster_nodes <= self.max_nr_cluster_nodes
        assert 1 <= self.min_nr_numa_nodes <= self.max_nr_numa_nodes
        assert 1 <= self.min_nr_threads <= self.max_nr_threads


    def __str__(self):
        return "Worker(type={}, pool={})" \
            .format(
                self.type,
                self.pool,
            )


    @property
    def nr_cluster_nodes_range(self):
        """
        Return range in number of cluster nodes to use
        """
        return self.max_nr_cluster_nodes - self.min_nr_cluster_nodes


    @property
    def nr_numa_nodes_range(self):
        """
        Return range in number of numa nodes to use
        """
        return self.max_nr_numa_nodes - self.min_nr_numa_nodes


    @property
    def nr_threads_range(self):
        """
        Return range in number of threads to use
        """
        return self.max_nr_threads - self.min_nr_threads


    @property
    def nr_benchmarks(self):
        """
        Return number of benchmarks to perform for benchmark
        """
        return self.pool.nr_permutations


    def nr_workers(self,
            benchmark_idx):
        """
        Return number of workers to use in benchmark with the index passed in
        """
        return self.pool.permutation_size(benchmark_idx)


    @property
    def nr_cluster_nodes(self):
        """
        Return number of nodes to use in benchmark

        This assumes that the range in number of nodes to use is zero
        """
        assert self.nr_cluster_nodes_range == 0

        return self.min_nr_cluster_nodes


    @property
    def nr_localities(self):
        """
        Return number of localities to use in benchmark

        This assumes that the range in number of nodes to use is zero
        """
        if self.type == "thread":
            assert self.nr_numa_nodes_range == 0
            assert self.nr_cluster_nodes_range == 0
            result = 1
        elif self.type == "numa_node":
            assert self.nr_numa_nodes_range == 0
            assert self.nr_cluster_nodes_range == 0
            result = self.min_nr_numa_nodes
        elif self.type == "cluster_node":
            # assert self.locality_per == "numa_node"
            assert self.nr_numa_nodes_range == 0
            assert self.nr_cluster_nodes_range == 0
            result = self.min_nr_cluster_nodes * self.min_nr_numa_nodes

        assert result > 0

        return result


    @property
    def nr_numa_nodes(self):
        """
        Return number of numa nodes to use in benchmark

        This assumes that the range in number of numa nodes to use is zero
        """
        assert self.nr_numa_nodes_range == 0, self

        return self.min_nr_numa_nodes


    @property
    def nr_threads(self):
        """
        Return number of threads to use in benchmark

        This assumes that the range in number of threads to use is zero
        """
        assert self.nr_threads_range == 0, self

        return self.min_nr_threads
