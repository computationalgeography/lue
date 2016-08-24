import os
import unittest
import lue
import lue_test


class DatasetTest(unittest.TestCase):
    """
    Verify the code used in the quick start section of the manual
    runs without issues.
    """


    def quick_start_create_new_dataset(self):

        lue_test.remove_file_if_existant("planets")

        planets_dataset = lue.create_dataset("planets")
        planet_phenomenon = planets_dataset.add_phenomenon("planet")
        omnipresent_properties = planet_phenomenon.add_property_set(
            "omnipresent")


    def test_quick_start_create_new_dataset(self):

        self.quick_start_create_new_dataset()


    def test_quick_start_query_existing_dataset(self):

        self.quick_start_create_new_dataset()

        planets_dataset = lue.open_dataset("planets")
        planets_dataset.phenomena.names
        planet_phenomenon = planets_dataset.phenomena["planet"]
        omnipresent_properties = planet_phenomenon.property_sets["omnipresent"]
