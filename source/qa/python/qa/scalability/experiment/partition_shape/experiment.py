from .. import experiment
from .. import shape
import os.path


class Experiment(experiment.Experiment):
    def __init__(self, data):

        super(Experiment, self).__init__(
            data,
            "partition_shape",
            data.get(
                "description",
                "Partition size scalability experiment of {} "
                "for increasing problem sizes, "
                "relating runtime to increasing partition sizes".format(
                    os.path.basename(data["command_pathname"])
                ),
            ),
        )

        self.from_json(data)

    def from_json(self, data):

        self.array = shape.Shape(data["array"])
        self.partition = shape.Shape(data["partition"])

    def to_json(self):

        result = super(Experiment, self).to_json()
        result["array"] = self.array.to_json()
        result["partition"] = self.partition.to_json()

        return result

    def benchmark_result_pathname(
        self,
        result_prefix,
        cluster_name,
        scenario_name,
        array_shape,
        basename,
        extension,
    ):

        return os.path.join(
            self.workspace_pathname(result_prefix, cluster_name, scenario_name),
            "x".join([str(extent) for extent in array_shape]),
            "{}.{}".format(basename, extension),
        )
