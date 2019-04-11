class Benchmark(object):

    def __init__(self, json):
        # Parse benchmark settings JSON
        self.count = json["count"]
        self.nr_workers = json["nr_workers"]
        self.worker_type = json["worker_type"]
        assert self.worker_type in ["thread", "node"]

        if self.worker_type == "thread":
            self.nr_nodes = 1
            self.nr_threads = self.nr_workers
        elif worker_type == "node":
            self.nr_nodes = self.nr_workers
            assert False
            self.nr_threads = json["nr_threads"]
