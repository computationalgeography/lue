from .. import experiment
from .. import shape
import os.path


class PartitionShapeExperiment(experiment.Experiment):

    def __init__(self,
            json,
            command_pathname):

        experiment.Experiment.__init__(self,
            json,
            "partition_shape",
            json.get(
                "description",
                "Partition size scaling experiment of {} "
                "for increasing problem sizes, "
                "relating runtime to increasing partition sizes"
                    .format(os.path.basename(command_pathname))
                ),
            command_pathname
        )

        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])


    def benchmark_result_pathname(self,
            cluster_name,
            array_shape,
            basename,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name),
            "x".join([str(extent) for extent in array_shape]),
            "{}.{}".format(basename, extension))
