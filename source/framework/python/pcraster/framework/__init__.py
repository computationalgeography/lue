"""
This module contains classes that mimic the behaviour of the PCRaster Python Framework.

See the `PCRaster Python Modelling Framework documentation`_ for more information.

.. _PCRaster Python Modelling Framework documentation:
   https://pcraster.geo.uu.nl/pcraster/latest/documentation/python_modelling_framework/index.html
"""
import lue.framework as lfr


class StaticModel(lfr.StaticModel):
    """
    Base class for static models
    """

    def __init__(self):
        super().__init__()

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

    def run(self):
        return super().run()


class DynamicModel(lfr.DynamicModel):
    """
    Base class for dynamic models
    """

    def __init__(self):
        super().__init__()

    def setDebug(self):
        # Skip for now
        pass

    def setQuiet(self, quiet=True):
        # Skip for now
        pass

    def currentTimeStep(self):
        # TODO
        pass

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
        super().__init__(model, last_time_step)

    def setQuiet(self, quiet=True):
        # Skip for now
        pass

    def run(self):
        return super().run()
