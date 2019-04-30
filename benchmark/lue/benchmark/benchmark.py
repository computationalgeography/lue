from . import worker


class Benchmark(object):

    def __init__(self, json, cluster):
        """
        Class for storing information about the configuration of the
        scaling experiment benchmarks

        count: How many times a benchmark needs to be repeated
        worker: Information about the workers to be used
        """

        self.count = json["count"]
        self.worker = worker.Worker(json["worker"], cluster)


    def __str__(self):
        return "Benchmark(count={}, worker={})" \
            .format(
                self.count,
                self.worker
            )
