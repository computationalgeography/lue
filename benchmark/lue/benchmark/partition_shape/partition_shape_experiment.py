from .. import experiment
from .. import shape
import os.path


class PartitionShapeExperiment(experiment.Experiment):

    def __init__(self,
            json):

        super(PartitionShapeExperiment, self).__init__(
            json,
            "partition_shape",
            json.get(
                "description",
                "Partition size scaling experiment of {} "
                "for increasing problem sizes, "
                "relating runtime to increasing partition sizes"
                    .format(os.path.basename(json["command_pathname"]))))

        self.from_json(json)

    def from_json(self, json):
        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])

    def to_json(self):
        result = super(PartitionShapeExperiment, self).to_json()
        result["array"] = self.array.to_json()
        result["partition"] = self.partition.to_json()

        return result

    def benchmark_result_pathname(self,
            cluster_name,
            scenario_name,
            array_shape,
            basename,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name, scenario_name),
            "x".join([str(extent) for extent in array_shape]),
            "{}.{}".format(basename, extension))
