from .. import experiment
from .. import shape
import os.path


class WeakScalingExperiment(experiment.Experiment):

    def __init__(self,
            json):

        super(WeakScalingExperiment, self).__init__(
            json,
            "weak_scaling",
            json.get(
                "description",
                "Weak scaling experiment of {}, "
                "relating runtime to an increasing number of workers, "
                "while keeping the problem size per worker constant"
                    .format(os.path.basename(json["command_pathname"]))))

        self.from_json(json)

    def from_json(self, json):
        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])

    def to_json(self):
        result = super(WeakScalingExperiment, self).to_json()
        result["array"] = self.array.to_json()
        result["partition"] = self.partition.to_json()

        return result

    def benchmark_result_pathname(self,
            cluster_name,
            scenario_name,
            nr_workers,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name, scenario_name),
            "{}.{}".format(nr_workers, extension))
