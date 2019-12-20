from .. import experiment
from .. import shape
import os.path


class WeakScalingExperiment(experiment.Experiment):

    def __init__(self,
            json,
            command_pathname):

        experiment.Experiment.__init__(self,
            json,
            "weak_scaling",
            json.get(
                "description",
                "Weak scaling experiment of {}, "
                "relating runtime to an increasing number of workers, "
                "while keeping the problem size per worker constant"
                    .format(os.path.basename(command_pathname))),
            command_pathname
        )

        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])


    def benchmark_result_pathname(self,
            cluster_name,
            scenario_name,
            nr_workers,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name, scenario_name),
            "{}.{}".format(nr_workers, extension))
