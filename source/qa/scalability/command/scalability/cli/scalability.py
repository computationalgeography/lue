import os.path
import sys

import docopt

from lue import __version__ as lue_version
from lue.command.scalability import perform_experiment_task
from lue.command.scalability.core import json_to_data


def main():
    usage = """\
Perform scalability experiment

Usage:
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        (script | import | postprocess) <configuration>
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        (script | import | postprocess)
        <command> <result_prefix> <cluster> <worker> <experiment> <script>
    {command} -h | --help
    {command} --version

Options:
    -h --help        Show this screen
    --version        Show version
""".format(
        command=os.path.basename(sys.argv[0])
    )

    arguments = docopt.docopt(usage, version=lue_version)

    if arguments["partition_shape"]:
        experiment = "partition_shape"
    elif arguments["weak_scalability"]:
        experiment = "weak_scalability"
    elif arguments["strong_scalability"]:
        experiment = "strong_scalability"
    else:
        experiment = None

    configuration_pathname = arguments["<configuration>"]

    if configuration_pathname is not None:
        configuration_data = json_to_data(configuration_pathname)
    else:
        configuration_data = {
            "command_pathname": "{}".format(arguments["<command>"]),
            "script_pathname": "{}".format(arguments["<script>"]),
            "result_prefix": "{}".format(arguments["<result_prefix>"]),
            "cluster": "{}".format(arguments["<cluster>"]),
            "benchmark": "{}".format(arguments["<worker>"]),
            "experiment": "{}".format(arguments["<experiment>"]),
        }

    if arguments["script"]:
        task = "script"
    elif arguments["import"]:
        task = "import"
    elif arguments["postprocess"]:
        task = "postprocess"
    else:
        task = None

    perform_experiment_task(experiment, task, configuration_data)
