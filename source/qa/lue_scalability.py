#!/usr/bin/env python
from lue import __version__ as lue_version
from lue.qa.scalability import perform_experiment_task
from lue.qa.scalability.core import json_to_data
import docopt
import os.path
import sys


usage = """\
Perform scalability experiment

Usage:
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        (script | import | postprocess) <configuration>
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        script <command> <result_prefix> <cluster> <worker> <experiment> <script>
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        import <command> <result_prefix> <cluster> <worker> <experiment> <script>
    {command}
        (partition_shape | weak_scalability | strong_scalability)
        postprocess <command> <result_prefix> <cluster> <worker> <experiment> <script>
    {command} -h | --help
    {command} --version

Options:
    -h --help        Show this screen
    --version        Show version
""".format(
    command = os.path.basename(sys.argv[0]))


if __name__ == "__main__":
    arguments = docopt.docopt(usage, version=lue_version)

    if arguments["partition_shape"]:
        experiment = "partition_shape"
    elif arguments["weak_scalability"]:
        experiment = "weak_scalability"
    elif arguments["strong_scalability"]:
        experiment = "strong_scalability"

    configuration_pathname = arguments["<configuration>"]

    if arguments["script"]:
        task = "script"
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
    elif arguments["import"]:
        task = "import"
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
    elif arguments["postprocess"]:
        task = "postprocess"
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

    perform_experiment_task(experiment, task, configuration_data)
