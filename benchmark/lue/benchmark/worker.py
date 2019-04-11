class Worker(object):

    def __init__(self, json):
        """
        Class for storing information about the workers to be used
        in scaling experiments

        Two types of workers can be added during an experiment:
        - threads: used in scaling experiments on a single node
        - node: used in scaling experiments on a cluster
        """

        self.type = json["type"]
        assert self.type in ["thread", "node"]

        self.min_nr = json["min_nr"]
        self.max_nr = json["max_nr"]

        assert self.min_nr > 0
        assert self.min_nr <= self.max_nr

        if self.type == "thread":
            # Scaling over threads. This only makes sense on a single node.
            self.min_nr_nodes = 1
            self.max_nr_nodes = 1
            self.min_nr_threads = self.min_nr
            self.max_nr_threads = self.max_nr
        elif self.type == "node":
            # Scaling over nodes. Within each node, all threads are used.
            self.min_nr_nodes = self.min_nr
            self.max_nr_nodes = self.max_nr
            assert False
            # Number of threads auto dectected, or obtain from json...
            # self.min_nr_threads = ...
            # self.max_nr_threads = ...


    def __str__(self):
        return "Worker(type={}, min_nr={}, max_nr={})" \
            .format(
                self.type,
                self.min_nr,
                self.max_nr,
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
        Return number of benchmark to perform for benchmark
        """
        return self.max_nr - self.min_nr + 1


    def nr_workers(self,
            benchmark_idx):
        """
        Return number of workers to use in benchmark with the index passed in
        """
        assert benchmark_idx in range(self.nr_benchmarks()), benchmark_idx

        return self.min_nr + benchmark_idx


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
