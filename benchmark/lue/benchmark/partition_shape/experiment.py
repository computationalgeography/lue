from .shape import partition_shapes
import os.path


class Experiment(object):

    def __init__(self,
            json,
            command_pathname):

        self.max_duration = json["max_duration"]
        self.max_tree_depth = json["max_tree_depth"]
        self.nr_time_steps = json["nr_time_steps"]
        self.array_shape = tuple(json["array_shape"])
        self.min_partition_shape = tuple(json["min_partition_shape"])
        self.max_partition_shape = tuple(json["max_partition_shape"])
        self.partition_shape_step = json["partition_shape_step"]
        self.command_pathname = command_pathname
        self.program_name = os.path.basename(self.command_pathname)
        self.description = json.get(
            "description",
            "Scaling experiment relating runtime of {} to partition size"
                .format(self.program_name))


    def partition_shapes(self):

        return partition_shapes(
            self.min_partition_shape, self.max_partition_shape,
            self.partition_shape_step)


    def result_basename(self,
            cluster_name):
        return "{}-{}".format(
            cluster_name,
            self.program_name)


    def benchmark_result_pathname(self,
            cluster_name,
            partition_shape):

        return "{}-{}.json".format(
            self.result_basename(cluster_name),
            "-".join([str(extent) for extent in partition_shape]))


    def result_pathname(self,
            cluster_name,
            extension,
            kind=None):

        basename = self.result_basename(cluster_name)

        if kind is not None:
            basename = "{}-{}".format(basename, kind)

        return "{}.{}".format(
            basename,
            extension)
