import os.path


class Experiment(object):

    def __init__(self,
            json,
            name,
            description):

        # Name (kind) of the experiment
        self.name = name

        self.description = description

        self.command_pathname = json["command_pathname"]
        self.max_duration = \
            json["max_duration"] if "max_duration" in json else None
        self.max_tree_depth = \
            json["max_tree_depth"] if "max_tree_depth" in json else None
        self.nr_time_steps = json["nr_time_steps"]

        self.program_name = os.path.basename(self.command_pathname)

    def to_json(self):
        result = {
                "command_pathname": self.command_pathname,
                "nr_time_steps": self.nr_time_steps,
            }

        if self.max_duration:
            result["max_duration"] = self.max_duration

        if self.max_tree_depth:
            result["max_tree_depth"] = self.max_tree_depth

        return result

    def workspace_pathname(self,
            cluster_name,
            scenario_name):
        """
        Return pathname of directory in which or below which all
        experiment results must be stored
        """
        return os.path.join(
            os.path.abspath(cluster_name),
            self.program_name,
            scenario_name,
            self.name)

    def result_pathname(self,
            cluster_name,
            scenario_name,
            basename,
            extension):

        return os.path.join(
            self.workspace_pathname(cluster_name, scenario_name),
            "{}.{}".format(basename, extension))

