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
        pass

    def setDebug(self):
        pass

    def readmap(self, name):
        pass

    def report(self, variable, name):
        pass

    def initial(self):
        pass


class StaticFramework(lfr.StaticModelRunner):
    """
    Class for running static models
    """

    def __init__(self, userModel):
        pass

    def run(self):
        pass


class DynamicModel(lfr.DynamicModel):
    """
    Base class for dynamic models
    """

    def __init__(self):
        pass

    def setDebug(self):
        pass

    def setQuiet(self, quiet=True):
        pass

    def currentTimeStep(self):
        pass

    def firstTimeStep(self):
        pass

    def nrTimeSteps(self):
        pass

    def timeSteps(self):
        pass

    def readmap(self, name):
        pass

    def report(self, variable, name):
        pass

    def initial(self):
        pass

    def dynamic(self):
        pass


class DynamicFramework(lfr.DynamicModelRunner):
    """
    Class for running dynamic models
    """

    def __init__(self, userModel, lastTimeStep=0, firstTimeStep=1):
        pass

    def setQuiet(self, quiet=True):
        pass

    def run(self):
        pass
