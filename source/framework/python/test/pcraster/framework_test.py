import inspect
import lue.pcraster.framework
import lue_test


class FrameworkTest(lue_test.TestCase):
    def inspect(self, class_name, member_function_names):
        classes = inspect.getmembers(lue.pcraster.framework, inspect.isclass)
        tuple_ = [tuple for tuple in classes if tuple[0] == class_name]

        self.assertTrue(tuple_, class_name)

        class_ = tuple_[0][1]
        functions = inspect.getmembers(class_, inspect.isfunction)

        for function_name in member_function_names:
            tuple_ = [tuple for tuple in functions if tuple[0] == function_name]
            self.assertTrue(tuple_, function_name)

    def test_static_model(self):
        self.inspect(
            "StaticModel",
            [
                "initial",
                "readmap",
                "report",
                "setDebug",
            ],
        )

    def test_static_framework(self):
        self.inspect(
            "StaticFramework",
            [
                "run",
            ],
        )

    def test_dynamic_model(self):
        self.inspect(
            "DynamicModel",
            [
                "currentTimeStep",
                "dynamic",
                "firstTimeStep",
                "initial",
                "nrTimeSteps",
                "setDebug",
                "setQuiet",
                "timeSteps",
            ],
        )

    def test_dynamic_framework(self):
        self.inspect(
            "DynamicFramework",
            [
                "run",
                "setQuiet",
            ],
        )
