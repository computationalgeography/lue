import os
import shutil
import sys

import lue.framework as lfr


class Progressor(lfr.Progressor):
    def __init__(self):
        lfr.Progressor.__init__(self)

    def preprocess(self, sample_nr):
        sys.stdout.write("[")
        sys.stdout.flush()

    def initialize(self):
        sys.stdout.write("[")
        sys.stdout.flush()

    def simulate(self, time_step):
        sys.stdout.write(".")
        sys.stdout.flush()

    def finalize(self):
        sys.stdout.write("]")
        sys.stdout.flush()

    def postprocess(self):
        sys.stdout.write("]")
        sys.stdout.flush()


class StaticModel(lfr.Model):
    def __init__(self):
        lfr.Model.__init__(self)

    # def run_initial(self):
    #     self.initial()

    def initialize(self):
        self.initial()


class StaticModelRunner(object):
    def __init__(self, model):
        self.model = model

    def run(self):
        # self.model.run_initial()
        lfr.run_deterministic(self.model, Progressor(), 0)


class DynamicModel(lfr.Model):
    def __init__(self):
        lfr.Model.__init__(self)

        self._current_time_step = 0

    def preprocess(self, sample_nr):
        self._current_sample_nr = sample_nr
        self.pre_monte_carlo()

    def initialize(self):
        self.initial()

    def simulate(self, time_step):
        self._current_time_step = time_step
        return self.dynamic()

    def postprocess(self):
        self.post_monte_carlo()

    # def run_initial(self):
    #     self.initial()

    # def run_dynamic(self):
    #     self.dynamic()

    @property
    def current_time_step(self) -> int:
        return self._current_time_step

    # @current_time_step.setter
    # def current_time_step(self, time_step: int):
    #     assert 0 < time_step, time_step
    #     self._current_time_step = time_step


class DynamicModelRunner(object):
    def __init__(self, model, last_time_step, first_time_step=1):
        assert first_time_step == 1, first_time_step

        self.model = model  # Inherits from lfr.Model
        self.last_time_step = last_time_step
        self.first_time_step = first_time_step
        assert (
            1 <= first_time_step <= last_time_step
        ), f"{first_time_step}, {last_time_step}"

    def run(self):
        assert self.first_time_step == 1, self.first_time_step
        lfr.run_deterministic(self.model, Progressor(), self.last_time_step)

        # self.model.run_initial()

        # for time_step in range(self.first_time_step, self.last_time_step + 1):
        #     self.model.current_time_step = time_step
        #     self.model.run_dynamic()


class MonteCarloModel(object):  # (lfr.Model):
    def __init__(self):
        self.first_sample_nr = 0
        self.last_sample_nr = 0
        self._current_sample_nr = 1

        # self.in_sample = False
        # self.in_stochastic = True
        # self.in_premc = False
        # self.in_postmc = False

    def set_sample_nrs(self, first_sample_nr, nr_samples):
        self.first_sample_nr = first_sample_nr
        self.last_sample_nr = first_sample_nr + nr_samples - 1
        self.current_sample_nr = self.first_sample_nr

    ### def preprocess(self, sample_nr):
    ###     self._current_sample_nr = sample_nr
    ###     self.pre_monte_carlo()

    ### def initialize(self):
    ###     self.initial()

    ### def simulate(self, time_step):
    ###     return self.dynamic()

    ### def postprocess(self, sample_nr):
    ###     self.post_monte_carlo()

    # def run_initial(self):
    #     self.initial()

    # def run_dynamic(self):
    #     self.dynamic()

    # def run_pre_monte_carlo(self):
    #     self.pre_monte_carlo()

    # def run_post_monte_carlo(self):
    #     self.post_monte_carlo()

    @property
    def current_sample_nr(self) -> int:
        return self._current_sample_nr

    @current_sample_nr.setter
    def current_sample_nr(self, sample_nr: int):
        assert 0 < sample_nr, sample_nr
        self._current_sample_nr = sample_nr

    # def nrSamples(self):
    #   """
    #   Return the number of samples
    #   """
    #   assert self._d_firstSampleNumber
    #   return self._d_lastSampleNumber - \
    #          self._d_firstSampleNumber + 1

    # def current_sample_nr(self):
    #   """
    #   Returns the current sample number
    #   """
    #   assert self._d_currentSampleNumber
    #   return self._d_currentSampleNumber

    # def sampleNumbers(self):
    #   """
    #   Returns a list of sample numbers configured
    #   """
    #   assert self._d_firstSampleNumber
    #   return range(self._d_firstSampleNumber, \
    #          self._d_lastSampleNumber + 1)

    # def _inStochastic(self):
    #   if not hasattr(self, "_d_inStochastic"):
    #     return False
    #   return self._d_inStochastic

    # def _inPremc(self):
    #   return self._d_inPremc

    # def _inPostmc(self):
    #   return self._d_inPostmc

    # def _lastSampleNumber(self):
    #   return self._d_lastSampleNumber

    # def _firstSampleNumber(self):
    #   return self._d_firstSampleNumber

    # def _setCurrentSample(self, nr):
    #   """
    #   Set the current sample number to nr.
    #   """
    #   assert nr >= self._firstSampleNumber()
    #   assert nr <= self._lastSampleNumber()
    #   self._d_currentSampleNumber = nr

    # def _inSample(self):
    #   """
    #   Return whether a sample is currently executing.
    #   """
    #   #if hasattr(self._userModel(), "_d_inSample"):
    #   return self._d_inSample
    #   #else:
    #   #  return False


class MonteCarloModelRunner(object):
    @staticmethod
    def initialize_sample_directories(model, remove_existing_directories):

        for sample_nr in range(model.first_sample_nr, model.last_sample_nr + 1):
            directory_pathname = f"{sample_nr}"

            if not os.path.isdir(directory_pathname):
                os.mkdir(directory_pathname)
            elif remove_existing_directories == True:
                shutil.rmtree(directory_pathname)
                os.mkdir(directory_pathname)

    def __init__(self, framework_model, nr_samples, remove_existing_directories):
        self.framework_model = framework_model
        self.nr_samples = nr_samples

        self.framework_model.model.set_sample_nrs(1, self.nr_samples)
        self.initialize_sample_directories(
            self.framework_model.model, remove_existing_directories
        )

    def run(self, rate_limit=0):

        assert rate_limit >= 0, rate_limit
        assert (
            self.framework_model.first_time_step == 1
        ), self.framework_model.first_time_step

        lfr.run_stochastic(
            self.framework_model.model,
            Progressor(),
            self.nr_samples,
            self.framework_model.last_time_step,
            rate_limit,
        )

        # self.model.model.run_pre_monte_carlo()

        # for sample_nr in range(
        #     self.model.model.first_sample_nr, self.model.model.last_sample_nr + 1
        # ):
        #     self.model.current_sample_nr = sample_nr
        #     self.model.run()

        # self.model.model.run_post_monte_carlo()
