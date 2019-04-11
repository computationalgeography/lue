from .. import experiment
from .. import shape
import os.path


class StrongScalingExperiment(experiment.Experiment):

    def __init__(self,
            json,
            command_pathname):

        experiment.Experiment.__init__(self,
            json,
            "strong_scaling",
            json.get(
                "description",
                "Scaling experiment relating runtime of {} to number or workers"
                    .format(os.path.basename(command_pathname))
                ),
            command_pathname
        )

        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])


    def benchmark_result_pathname(self,
            cluster_name,
            nr_workers,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name),
            "{}.{}".format(nr_workers, extension))
