import unittest

import lue.qa.scalability.instrument as lqi


class ExperimentTest(unittest.TestCase):
    def test_use_case_01(self):

        nr_workers = 5
        experiment = lqi.Experiment(nr_workers)

        self.assertEqual(experiment.nr_workers, nr_workers)

        experiment.start()
        experiment.stop()
