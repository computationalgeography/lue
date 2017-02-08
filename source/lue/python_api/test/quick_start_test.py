import lue
import lue_test


class QuickStartTest(lue_test.TestCase):
    """
    Verify the code used in the quick start section of the manual
    runs without issues.
    """


    def quick_start_create_new_dataset(self):

        lue_test.remove_file_if_existant("planets")

        dataset = lue.create_dataset("planets")
        planets = dataset.add_phenomenon("planets")
        properties = planets.add_property_set("omnipresent")


    def test_quick_start_create_new_dataset(self):

        self.quick_start_create_new_dataset()


    def test_quick_start_query_existing_dataset(self):

        self.quick_start_create_new_dataset()

        dataset = lue.open_dataset("planets", "r")
        dataset.phenomena.names
        planets = dataset.phenomena["planets"]
        properties = planets.property_sets["omnipresent"]
