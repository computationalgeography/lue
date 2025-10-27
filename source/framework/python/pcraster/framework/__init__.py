"""
This module contains classes that mimic the behaviour of the PCRaster Python Framework.

See the `PCRaster Python Modelling Framework documentation`_ for more information.

.. _PCRaster Python Modelling Framework documentation:
   https://pcraster.geo.uu.nl/pcraster/latest/documentation/python_modelling_framework/index.html
"""

import os.path
import pathlib
from typing import Union

import lue.framework as lfr


class StaticModel(lfr.StaticModel):
    """
    Base class for static models
    """

    # def __init__(self):
    #     super().__init__()

    def setDebug(self):
        # Skip for now
        pass

    def readmap(self, name):
        # TODO
        pass

    def report(self, variable, name):
        # TODO
        pass

    def initial(self):
        raise NotImplementedError


class StaticFramework(lfr.StaticModelRunner):
    """
    Class for running static models
    """

    def __init__(self, model):
        super().__init__(model)


class DynamicModel(lfr.DynamicModel):
    """
    Base class for dynamic models
    """

    # def __init__(self):
    #     super().__init__()

    def setDebug(self):
        # Skip for now
        pass

    def setQuiet(self, quiet=True):
        # Skip for now
        pass

    def currentTimeStep(self):
        return super().current_time_step

    def firstTimeStep(self):
        # TODO
        pass

    def nrTimeSteps(self):
        # TODO
        pass

    def timeSteps(self):
        # TODO
        pass

    def readmap(self, name):
        # TODO
        pass

    def report(self, variable, name):
        # TODO
        pass

    def initial(self):
        raise NotImplementedError

    def dynamic(self):
        raise NotImplementedError


class DynamicFramework(lfr.DynamicModelRunner):
    """
    Class for running dynamic models
    """

    def __init__(self, model, last_time_step=0, first_time_step=1):
        super().__init__(model, last_time_step, first_time_step)
        self.quiet = False

    def setQuiet(self, quiet=True):
        self.quiet = quiet

    def run(self, *, rate_limit=0):
        if self.quiet:
            progressor = lfr.SilentProgressor()
        else:
            progressor = lfr.DefaultProgressor()
        super().run(progressor=progressor, rate_limit=rate_limit)


class MonteCarloModel(lfr.MonteCarloModel):
    """
    Base class for stochastic models
    """

    # def __init__(self):
    #     super().__init__()

    def pre_monte_carlo(self):
        self.premcloop()

    def premcloop(self):
        raise NotImplementedError

    def post_monte_carlo(self):
        self.premcloop()

    def postmcloop(self):
        raise NotImplementedError

    # def nrSamples(self):
    #   """
    #   Return the number of samples
    #   """
    #   assert self._d_firstSampleNumber
    #   return self._d_lastSampleNumber - \
    #          self._d_firstSampleNumber + 1

    def currentSampleNumber(self):
        return super().current_sample_nr

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


class MonteCarloFramework(lfr.MonteCarloModelRunner):
    """
    Class for running stochastic models
    """

    def __init__(self, model, nr_samples=0, remove_dirs=True):
        super().__init__(model, nr_samples, remove_dirs)

    def setForkSamples(self, fork, nr_cpus=1):
        pass

    def setQuiet(self, quiet=True):
        # Skip for now
        pass

    # def run(self):
    #     return super().run()


#     Return the class provided by the user.
#     """
#     return self._d_model._userModel()
#
#   def _userFramework(self):
#     """
#     Return the framework provided by the user.
#     """
#     return self._d_model
#
#   def _testRequirements(self):
#     """
#     Test whether the user model models the
#     :ref:`Monte Carlo Model Concept <monteCarloModelConcept>`.
#
#     .. todo::
#
#        The implementation uses the staticFramework and dynamicFramework modules,
#        which makes this class dependent on these specific framework classes.
#        This is not necessary. The implementation should just perform the
#        concept check and be done with it. Only use the interface defined by
#        the concept, and nothing more. Don't assume anything about the
#        implementation.
#     """
#     if (not isinstance(self._userFramework(), staticFramework.StaticFramework)) and (not isinstance(self._userFramework(), dynamicFramework.DynamicFramework)):
#       msg = "Cannot run MonteCarlo framework: User model must be type of \
# StaticFramework or DynamicFramework"
#       raise frameworkBase.FrameworkError(msg)
#
#     if not hasattr(self._userModel(), "premcloop"):
#       self.showWarning("No premcloop section defined.")
#
#     if not hasattr(self._userModel(), "postmcloop"):
#       self.showWarning("No postmcloop section defined.")
#
#   ## \brief Re-implemented from ShellScript.
#   #
#   # Runs the user model in the Monte Carlo mode.
#   def run(self, premc=True, postmc=True):
#     self._atStartOfScript()
#     self._check()
#
#     if premc:
#       self._runPremcloop()
#
#     # Samples.
#     sample = self._userModel()._firstSampleNumber()
#     while sample <= self._userModel()._lastSampleNumber():
#
#       if self._forkSamples():
#         assert self.isParentProcess()
#         while self._systemIsOccupied(self.nrChildProcesses()):
#           if self._debug():
#             self.showMessage("System is occupied")
#           # Wait for some seconds.
#           # But first handle finished children.
#           for childProcess in self.handleFinishedChildProcesses():
#             if not self._quiet():
#               if self._debug():
#                 self.showMessage(childProcess.message())
#           time.sleep(0.1)
#
#         # Throw in another sample.
#         if self._debug():
#           self.showMessage("Starting sample %d" % (sample))
#         self.fork(str(sample))
#
#         if self.isParentProcess():
#           # Stop parent process to enable child process to start and
#           # the load to adjust.
#           # But first handle finished children.
#           for childProcess in self.handleFinishedChildProcesses():
#             if not self._quiet():
#               if self._debug():
#                 self.showMessage(childProcess.message())
#           time.sleep(0.01)
#
#       # Child processes start here.
#       if not self._forkSamples() or \
#          (self._forkSamples() and self.isChildProcess()):
#         self._incrementIndentLevel()
#         self._atStartOfSample(sample)
#         self._userModel()._setCurrentSample(sample)
#         # Execute model
#         self._d_model.run()
#
#         self._sampleFinished()
#         self._decrementIndentLevel()
#
#       # Child processes stop here.
#       if self._forkSamples():
#         if self.isChildProcess():
#           os._exit(0)
#
#       assert not self._forkSamples() or self.isParentProcess()
#       sample += 1
#
#     if self._forkSamples() and  self.isParentProcess():
#       for childProcess in self.waitForChildProcessesToFinish():
#         if not self._quiet():
#           if self._debug():
#             self.showMessage(childProcess.message())
#       assert not self.childProcesses()
#
#     if postmc:
#       self._runPostmcloop()
#
#     return 0


class FrameworkError(Exception):
    def __init__(self, message):
        self._msg = message

    def __str__(self):
        return self._msg


def generateNameT(name: Union[str, bytes, os.PathLike], timestep: int) -> str:
    """
    Return a filename based on the name and time step passed in.

    The time step normally ranges from [1, nrTimeSteps].

    The name passed in may contain a directory name.

    See also: generateNameS(), generateNameST()
    """
    path = pathlib.Path(name)

    if len(name) == 0:
        raise RuntimeError("No filename specified")

    if timestep < 1:
        msg = f"Timestep ('{timestep}') must be larger than 0"
        raise RuntimeError(msg)

    parent = path.parent
    filename = path.name
    suffix = path.suffix

    if suffix != "":
        msg = f"File extension given in '{filename}' not allowed"
        raise RuntimeError(msg)

    filename = f"{filename}_{timestep}.tif"

    return f"{pathlib.Path(parent, filename)}"


def generateNameS(name, sample):
    """Return a filename based on the name and sample number passed in.

    The resulting name contains a directory and a filename part. The
    sample number is used as the directory name and the name is used as
    the filename.

    The sample number normally ranges from [1, nrSamples].

    See also: generateNameT(), generateNameST()
    """
    return os.path.join("%d" % (sample), name)


def generateNameST(name, sample, timestep):
    """
    Return a filename based on the name, sample number and time step.

    See also: generateNameT(), generateNameS()
    """
    return generateNameS(generateNameT(name, timestep), sample)
