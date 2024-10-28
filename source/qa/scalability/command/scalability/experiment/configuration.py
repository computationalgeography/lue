import os.path

from ..core import json_to_data
from .benchmark import Benchmark
from .cluster import Cluster


class Configuration(object):
    def __init__(self, data):
        self._from_json(data)

    def _from_json(self, data):
        self.command_pathname = os.path.expandvars(data["command_pathname"])
        self.command_arguments = data["command_arguments"]
        self.script_pathname = os.path.expandvars(data["script_pathname"])
        self.result_prefix = os.path.expandvars(data["result_prefix"])
        self.cluster = Cluster(json_to_data(data["cluster"]))
        self.benchmark = Benchmark(json_to_data(data["benchmark"]), self.cluster)
