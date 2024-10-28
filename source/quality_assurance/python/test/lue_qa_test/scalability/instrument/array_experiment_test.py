import unittest

import lue.qa.scalability.instrument as lqi


class ArrayExperimentTest(unittest.TestCase):
    def test_use_case_01(self):

        nr_workers = 5
        array_shape = (60, 40)
        partition_shape = (10, 10)
        experiment = lqi.ArrayExperiment(nr_workers, array_shape, partition_shape)

        self.assertEqual(experiment.nr_workers, nr_workers)
        self.assertEqual(experiment.array_shape, array_shape)
        self.assertEqual(experiment.partition_shape, partition_shape)

        experiment.start()
        experiment.stop()
