import os.path


class Experiment(object):

    def __init__(self,
            json,
            name,
            description,
            command_pathname):

        # Name (kind) of the experiment
        self.name = name

        self.description = description
        self.command_pathname = command_pathname
        self.program_name = os.path.basename(self.command_pathname)

        self.max_duration = \
            json["max_duration"] if "max_duration" in json else None
        self.max_tree_depth = json["max_tree_depth"]
        self.nr_time_steps = json["nr_time_steps"]

    def workspace_pathname(self,
            cluster_name):
        """
        Return pathname of directory in which or below which all
        experiment results must be stored
        """
        return os.path.join(
            os.path.abspath(cluster_name),
            self.program_name,
            self.name)


    def result_pathname(self,
            cluster_name,
            basename,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name),
            "{}.{}".format(basename, extension))
