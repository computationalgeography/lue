class StaticModel(object):
    def run_initial(self):
        self.initial()


class StaticModelRunner(object):
    def __init__(self, model):
        self.model = model

    def run(self):
        self.model.run_initial()


class DynamicModel(object):
    def run_initial(self):
        self.initial()

    def run_dynamic(self):
        self.dynamic()


class DynamicModelRunner(object):
    def __init__(self, model, last_time_step):
        self.model = model
        self.last_time_step = last_time_step
        self.first_time_step = 1

    def run(self):
        self.model.run_initial()

        for t in range(self.first_time_step, self.last_time_step):
            self.model.run_dynamic()
