import os.path


class Arguments(object):

    def __init__(self,
            json_dict):

        self.positionals = json_dict["positionals"] if "positionals" in json_dict else []
        self.options = json_dict["options"] if "options" in json_dict else {}


    @property
    def to_list(self):
        result = []

        for positional in self.positionals:
            result.append('"{}"'.format(positional))

        for key, value in self.options.items():
            result.append('--{}="{}"'.format(key, value) if value else '--{}'.format(key))

        return result


class Experiment(object):

    def __init__(self,
            json,
            name,
            description):

        # Name (kind) of the experiment
        self.name = name

        self.description = description
        self.command_pathname = json["command_pathname"]
        self.arguments = Arguments(json["arguments"]) if "arguments" in json else Arguments({})

        self.max_duration = json["max_duration"] if "max_duration" in json else None
        self.max_tree_depth = json["max_tree_depth"] if "max_tree_depth" in json else None
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


    @property
    def argument_list(self):
        return self.arguments.to_list


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
