#!/usr/bin/env python
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

Options:
    -h --help        Show this screen
""".format(
    command = os.path.basename(sys.argv[0]))


if __name__ == "__main__":
    arguments = docopt.docopt(usage)

    if arguments["partition_shape"]:
        experiment = "partition_shape"
    elif arguments["weak_scalability"]:
        experiment = "weak_scalability"
    elif arguments["strong_scalability"]:
        experiment = "strong_scalability"

    if arguments["script"]:
        task = "script"
    elif arguments["import"]:
        task = "import"
    elif arguments["postprocess"]:
        task = "postprocess"

    configuration_pathname = arguments["<configuration>"]
    configuration_data = json_to_data(configuration_pathname)

    perform_experiment_task(experiment, task, configuration_data)
