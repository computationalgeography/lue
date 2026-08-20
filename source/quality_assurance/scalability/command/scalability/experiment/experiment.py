import os.path


class Arguments:
    def __init__(self, json_dict):
        self.positionals = json_dict.get("positionals", [])
        self.options = json_dict.get("options", {})

    @property
    def to_list(self):
        result = []

        for positional in self.positionals:
            result.append(f'"{positional}"')

        for key, value in self.options.items():
            result.append(f'--{key}="{value}"' if value else f"--{key}")

        return result


class Experiment:
    def __init__(self, data, name, description):
        # Name (kind) of the experiment
        self.name = name

        self.description = description
        self.command_pathname = data["command_pathname"]
        self.command_arguments = data["command_arguments"]
        self.arguments = (
            Arguments(data["arguments"]) if "arguments" in data else Arguments({})
        )

        self.max_duration = data.get("max_duration", None)
        self.max_tree_depth = data.get("max_tree_depth", None)
        # self.nr_time_steps = data["nr_time_steps"]

        self.program_name = os.path.basename(self.command_pathname)

    def to_json(self):
        result = {
            "command_pathname": self.command_pathname,
            "command_arguments": self.command_arguments,
            # "nr_time_steps": self.nr_time_steps,
        }

        if self.max_duration:
            result["max_duration"] = self.max_duration

        if self.max_tree_depth:
            result["max_tree_depth"] = self.max_tree_depth

        return result

    @property
    def argument_list(self):
        return self.arguments.to_list

    def workspace_pathname(self, result_prefix, cluster_name, scenario_name):
        """
        Return pathname of directory in which or below which all
        experiment results must be stored
        """
        return os.path.join(
            os.path.abspath(result_prefix),
            cluster_name,
            self.program_name,
            scenario_name,
            self.name,
        )

    def result_pathname(
        self, result_prefix, cluster_name, scenario_name, basename, extension=""
    ):
        return os.path.join(
            self.workspace_pathname(result_prefix, cluster_name, scenario_name),
            f"{basename}.{extension}" if extension else basename,
        )
