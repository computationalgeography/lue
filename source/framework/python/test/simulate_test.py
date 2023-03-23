import time

import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class MyModel(lfr.Model):
    def __init__(self):
        lfr.Model.__init__(self)
        self.preprocess_called = 0
        self.initialize_called = 0
        self.simulate_called = 0
        self.finalize_called = 0
        self.postprocess_called = 0

    def preprocess(self, sample_nr):
        self.preprocess_called += 1

    def initialize(self):
        self.initialize_called += 1

    def simulate(self, time_step):
        # This sleep is relevant. Otherwise things go bad in the simulate function.
        # Possibly tasks get shuffled(?).
        time.sleep(0.1)
        self.simulate_called += 1

    def finalize(self):
        self.finalize_called += 1

    def postprocess(self):
        self.postprocess_called += 1


class MyProgressor(lfr.Progressor):
    def __init__(self):
        lfr.Progressor.__init__(self)
        self.preprocess_called = 0
        self.initialize_called = 0
        self.simulate_called = 0
        self.finalize_called = 0
        self.postprocess_called = 0

    def preprocess(self, sample_nr):
        self.preprocess_called += 1

    def initialize(self):
        self.initialize_called += 1

    def simulate(self, time_step):
        self.simulate_called += 1

    def finalize(self):
        self.finalize_called += 1

    def postprocess(self):
        self.postprocess_called += 1


class SimulateTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_run_deterministic(self):
        model = MyModel()
        progressor = MyProgressor()
        nr_time_steps = 5

        lfr.run_deterministic(model, progressor, nr_time_steps, rate_limit=2)

        self.assertEqual(model.preprocess_called, 0)
        self.assertEqual(model.initialize_called, 1)
        self.assertEqual(model.simulate_called, nr_time_steps)
        self.assertEqual(model.finalize_called, 1)
        self.assertEqual(model.postprocess_called, 0)

        self.assertEqual(progressor.preprocess_called, 0)
        self.assertEqual(progressor.initialize_called, 1)
        self.assertEqual(progressor.simulate_called, nr_time_steps)
        self.assertEqual(progressor.finalize_called, 1)
        self.assertEqual(progressor.postprocess_called, 0)

    @lue_test.framework_test_case
    def test_run_stochastic(self):
        model = MyModel()
        progressor = MyProgressor()
        nr_samples = 3
        nr_time_steps = 5

        lfr.run_stochastic(model, progressor, nr_samples, nr_time_steps)

        self.assertEqual(model.preprocess_called, nr_samples)
        self.assertEqual(model.initialize_called, nr_samples)
        self.assertEqual(model.simulate_called, nr_samples * nr_time_steps)
        self.assertEqual(model.finalize_called, nr_samples)
        self.assertEqual(model.postprocess_called, nr_samples)

        self.assertEqual(progressor.preprocess_called, nr_samples)
        self.assertEqual(progressor.initialize_called, nr_samples)
        self.assertEqual(progressor.simulate_called, nr_samples * nr_time_steps)
        self.assertEqual(progressor.finalize_called, nr_samples)
        self.assertEqual(progressor.postprocess_called, nr_samples)
