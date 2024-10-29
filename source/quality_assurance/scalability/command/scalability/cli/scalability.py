import datetime
import sys
from pathlib import Path

import docopt

from lue import __version__ as lue_version
from lue.command.scalability import perform_experiment_task


def main():
    usage = """\
Perform scalability experiment

Usage:
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        (script | import | postprocess)
        <command> <arguments>
        <cluster> <worker>
        <cluster_config_prefix> <experiment_config_prefix> <result_prefix>
    {command} -h | --help
    {command} --version

Options:
    -h --help       Show this screen
    --version       Show version
    command         Command to analyse
    arguments       Additional arguments to pass on to the command. Pass an
                    empty string if this is not needed.
    result_prefix   Directory to store results in
    cluster         Path to json file containing info about the cluster
                    (or node)
""".format(
        command=Path(sys.argv[0]).name
    )

    arguments = docopt.docopt(usage, version=lue_version)

    if arguments["partition_shape"]:
        experiment_name = "partition_shape"
    elif arguments["weak_scalability"]:
        experiment_name = "weak_scalability"
    elif arguments["strong_scalability"]:
        experiment_name = "strong_scalability"
    else:
        experiment_name = None

    if arguments["script"]:
        task = "script"
    elif arguments["import"]:
        task = "import"
    elif arguments["postprocess"]:
        task = "postprocess"
    else:
        task = None

    command_name = arguments["<command>"]
    command_arguments = arguments["<arguments>"]
    cluster_name = arguments["<cluster>"]
    worker_name = arguments["<worker>"]

    cluster_config_prefix = arguments["<cluster_config_prefix>"]
    experiment_config_prefix = arguments["<experiment_config_prefix>"]
    result_prefix = arguments["<result_prefix>"]

    script_pathname = Path(result_prefix).joinpath(
        cluster_name, worker_name, command_name, f"{experiment_name}.sh"
    )
    cluster_config_pathname = Path(cluster_config_prefix).joinpath(
        cluster_name, "cluster.json"
    )
    worker_config_pathname = Path(cluster_config_prefix).joinpath(
        cluster_name, worker_name, f"{experiment_name}.json"
    )
    experiment_config_pathname = Path(experiment_config_prefix).joinpath(
        cluster_name, worker_name, command_name, f"{experiment_name}.json"
    )

    result_path = Path(result_prefix).joinpath(
        cluster_name, worker_name, command_name, experiment_name
    )

    if task == "script" and result_path.exists():
        modification_time_point = datetime.datetime.fromtimestamp(
            result_path.stat().st_mtime
        )

        result_path.rename(
            f"{result_path}-{modification_time_point.isoformat(timespec='seconds')}"
        )

    configuration_data = {
        "command_pathname": f"{command_name}",
        "command_arguments": f"{command_arguments}",
        "script_pathname": f"{script_pathname}",
        "result_prefix": f"{result_prefix}",
        "cluster": f"{cluster_config_pathname}",
        "benchmark": f"{worker_config_pathname}",  # TODO worker?
        "experiment": f"{experiment_config_pathname}",
    }

    perform_experiment_task(experiment_name, task, configuration_data)
