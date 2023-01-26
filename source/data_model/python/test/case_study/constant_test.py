# -*- encoding: utf-8 -*-
import lue.data_model as ldm
import lue_test
import numpy as np


class ConstantTest(lue_test.TestCase):
    def test_case_study(self):

        dataset = ldm.create_dataset("planets.lue")
        planets = dataset.add_phenomenon("planets")

        nr_planets = 3
        planets.object_id.expand(nr_planets)[:] = np.arange(nr_planets)

        constants = planets.add_property_set("constants")

        # TODO Fix as part of GH #447
        ### name = constants.add_property(
        ###     "name", dtype=np.unicode_)
        ### name.value.expand(nr_planets)[:] = \
        ###     np.array([
        ###             # Python 2: Byte string, encoded in UTF8
        ###             # Python 3: Unicode string
        ###             "ñeptùne",
        ###             "mærß",
        ###             "ùræñùß"
        ###         ])

        ### gravity = constants.add_property(
        ###     "gravity", dtype=np.float32)
        ### gravity.value.expand(nr_planets)[:] = \
        ###     np.array([1.5, 2.5, 3.5], dtype=np.float32)

        ldm.assert_is_valid(dataset)
