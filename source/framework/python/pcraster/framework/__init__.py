import lue.framework as lfr


class StaticModel(lfr.StaticModel):
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
    def __init__(self, userModel):
        pass

    def run(self):
        pass


class DynamicModel(lfr.DynamicModel):
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
    def __init__(self, userModel, lastTimeStep=0, firstTimeStep=1):
        pass

    def setQuiet(self, quiet=True):
        pass

    def run(self):
        pass
