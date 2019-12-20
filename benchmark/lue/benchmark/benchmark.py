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
        worker: Information about the workers to be used
        """
        self.scenario_name = json["scenario"] if "scenario" in json else None
        self.count = json["count"]
        self.worker = worker.Worker(json["worker"], cluster)
        self.hpx = HPX(json["hpx"]) if "hpx" in json else None


    def __str__(self):
        return "Benchmark(count={}, worker={})" \
            .format(
                self.count,
                self.worker
            )
