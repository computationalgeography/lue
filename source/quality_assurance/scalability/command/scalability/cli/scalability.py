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
        <platform> (core_numa_node | core_cluster_node | numa_node | cluster_node)
        <platform_config_prefix> <experiment_config_prefix> <result_prefix>
    {command} -h | --help
    {command} --version

Options:
    -h --help       Show this screen
    --version       Show version
    command         Command to analyse
    arguments       Additional arguments to pass on to the command. Pass an
                    empty string if this is not needed.
    result_prefix   Directory to store results in
    platform        Name of platform
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
        experiment_name = "unknown"

    if arguments["script"]:
        task_name = "script"
    elif arguments["import"]:
        task_name = "import"
    elif arguments["postprocess"]:
        task_name = "postprocess"
    else:
        task_name = "unknown"

    if arguments["core_numa_node"]:
        worker_name = "core_numa_node"
    elif arguments["core_cluster_node"]:
        worker_name = "core_cluster_node"
    elif arguments["numa_node"]:
        worker_name = "numa_node"
    elif arguments["cluster_node"]:
        worker_name = "cluster_node"
    else:
        worker_name = "unknown"

    command_name = arguments["<command>"]
    command_arguments = arguments["<arguments>"]
    platform_name = arguments["<platform>"]

    platform_config_prefix = arguments["<platform_config_prefix>"]
    experiment_config_prefix = arguments["<experiment_config_prefix>"]
    result_prefix = arguments["<result_prefix>"]

    script_pathname = Path(result_prefix).joinpath(
        platform_name, command_name, worker_name, f"{experiment_name}.sh"
    )
    platform_config_pathname = Path(platform_config_prefix).joinpath(
        platform_name, "platform.json"
    )
    worker_config_pathname = Path(platform_config_prefix).joinpath(
        platform_name, worker_name, f"{experiment_name}.json"
    )
    experiment_config_pathname = Path(experiment_config_prefix).joinpath(
        platform_name, command_name, worker_name, f"{experiment_name}.json"
    )

    result_path = Path(result_prefix).joinpath(
        platform_name, command_name, worker_name, experiment_name
    )

    if task_name == "script" and result_path.exists():
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
        "platform": f"{platform_config_pathname}",
        "benchmark": f"{worker_config_pathname}",  # TODO worker?
        "experiment": f"{experiment_config_pathname}",
    }

    # import pprint
    # pprint.pprint(configuration_data)
    # meh

    perform_experiment_task(experiment_name, task_name, configuration_data)
