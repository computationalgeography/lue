from . import shape
import os.path


class Experiment(object):

    def __init__(self,
            json,
            name,
            command_pathname):

        self.max_duration = json["max_duration"]
        self.max_tree_depth = json["max_tree_depth"]
        self.nr_time_steps = json["nr_time_steps"]

        self.array = shape.Shape(json["array"])
        self.partition = shape.Shape(json["partition"])

        self.name = name

        self.command_pathname = command_pathname
        self.program_name = os.path.basename(self.command_pathname)
        self.description = json.get(
            "description",
            "Scaling experiment relating runtime of {} to partition size"
                .format(self.program_name))


    def benchmark_result_pathname(self,
            cluster_name,
            array_shape,
            basename,
            extension):

        directory_pathname = os.path.join(
            os.path.abspath(cluster_name),
            self.name,
            self.program_name,
            "x".join([str(extent) for extent in array_shape]))

        return "{}/{}.{}".format(
            directory_pathname,
            basename,
            extension)


    def result_pathname(self,
            cluster_name,
            basename,
            extension):

        directory_pathname = os.path.join(
            os.path.abspath(cluster_name),
            self.name,
            self.program_name)

        return "{}/{}.{}".format(
            directory_pathname,
            basename,
            extension)
