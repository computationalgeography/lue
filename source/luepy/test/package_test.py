import lue
import lue_test


class PackageTest(lue_test.TestCase):


    def test_version(self):

        self.assertTrue(hasattr(lue, "__version__"))
        self.assertTrue(lue.__version__)
