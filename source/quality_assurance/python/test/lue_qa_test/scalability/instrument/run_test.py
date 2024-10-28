import unittest

import lue.qa.scalability.instrument as lqi


class RunTest(unittest.TestCase):
    def test_use_case_01(self):

        run = lqi.Run()

        run.start()
        run.stop()
