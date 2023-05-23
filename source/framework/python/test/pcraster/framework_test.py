import inspect

import lue.pcraster.framework
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


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

    @lue_test.framework_test_case
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

    @lue_test.framework_test_case
    def test_static_framework(self):
        self.inspect(
            "StaticFramework",
            [
                "run",
            ],
        )

    @lue_test.framework_test_case
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

    @lue_test.framework_test_case
    def test_dynamic_framework(self):
        self.inspect(
            "DynamicFramework",
            [
                "run",
                "setQuiet",
            ],
        )

    @lue_test.framework_test_case
    def test_static_model_use_case(self):
        class UserModel(lue.pcraster.framework.StaticModel):
            def __init__(self):
                super().__init__()
                self.initialized = False

            def initial(self):
                self.initialized = True

        my_model = UserModel()
        static_model = lue.pcraster.framework.StaticFramework(my_model)
        static_model.run()

        self.assertTrue(my_model.initialized)

    @lue_test.framework_test_case
    def test_dynamic_model_use_case(self):
        class UserModel(lue.pcraster.framework.DynamicModel):
            def __init__(self):
                super().__init__()
                self.initialized = False
                self.nr_iterations = 0

            def initial(self):
                self.initialized = True

            def dynamic(self):
                self.nr_iterations += 1

        my_model = UserModel()
        dynamic_model = lue.pcraster.framework.DynamicFramework(my_model, 10)
        dynamic_model.run()

        self.assertTrue(my_model.initialized)
        self.assertEqual(my_model.nr_iterations, 10)
