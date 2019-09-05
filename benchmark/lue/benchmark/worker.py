from . import pool


class Worker(object):

    def __init__(self, data, cluster):
        """
        Class for storing information about the workers to be used
        in scaling experiments

        Two types of workers can be added during an experiment:
        - threads: used in scaling experiments on a single node
        - node: used in scaling experiments on a cluster
        """

        self.type = data["type"]
        assert self.type in ["thread", "node"]

        self.pool = pool.Pool(data["pool"])

        if self.type == "thread":
            # Scaling over threads. This only makes sense on a single node.
            self.min_nr_nodes = 1
            self.max_nr_nodes = 1
            self.min_nr_threads = self.pool.min_size
            self.max_nr_threads = self.pool.max_size
        elif self.type == "node":
            # Scaling over nodes. Within each node, all threads are used.
            self.min_nr_nodes = self.pool.min_size
            self.max_nr_nodes = self.pool.max_size
            self.min_nr_threads = cluster.node.nr_threads()
            self.max_nr_threads = self.min_nr_threads


    def __str__(self):
        return "Worker(type={}, pool={})" \
            .format(
                self.type,
                self.pool,
            )


    def nr_nodes_range(self):
        """
        Return range in number of nodes to use
        """
        return self.max_nr_nodes - self.min_nr_nodes


    def nr_threads_range(self):
        """
        Return range in number of threads to use
        """
        return self.max_nr_threads - self.min_nr_threads


    def nr_benchmarks(self):
        """
        Return number of benchmarks to perform for benchmark
        """
        return self.pool.nr_permutations()


    def nr_workers(self,
            benchmark_idx):
        """
        Return number of workers to use in benchmark with the index passed in
        """
        return self.pool.permutation_size(benchmark_idx)


    def nr_nodes(self):
        """
        Return number of nodes to use in benchmark

        This assumes that the range in number of nodes to use is zero
        """
        assert self.nr_nodes_range() == 0

        return self.min_nr_nodes


    def nr_threads(self):
        """
        Return number of threads to use in benchmark

        This assumes that the range in number of threads to use is zero
        """
        assert self.nr_threads_range() == 0, self

        return self.min_nr_threads
