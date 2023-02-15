from lue.qa.scalability.core import json_to_data

from .. import configuration
from . import experiment


class Configuration(configuration.Configuration):
    def __init__(self, data):

        super(Configuration, self).__init__(data)
        self.from_json(data)

    def from_json(self, data):

        experiment_data = json_to_data(data["experiment"])
        experiment_data["command_pathname"] = self.command_pathname
        self.experiment = experiment.Experiment(experiment_data)
