from .. import experiment
from .. import shape
import os.path


class StrongScalingExperiment(experiment.Experiment):

    def __init__(self,
            json):

        super(StrongScalingExperiment, self).__init__(
            json,
            "strong_scaling",
            json.get(
                "description",
                "Strong scaling experiment of {}, "
                "relating runtime to an increasing number of workers, "
                "while keeping the total problem size fixed"
                    .format(os.path.basename(json["command_pathname"]))))

        self.from_json(json)

    def from_json(self, json):
        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])

    def to_json(self):
        result = super(StrongScalingExperiment, self).to_json()
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
