# -*- encoding: utf-8 -*-
import lue
import lue_test
import numpy as np


class ConstantTest(lue_test.TestCase):

    def test_case_study(self):

        dataset = lue.create_dataset("planets.lue")
        planets = dataset.add_phenomenon("planets")

        nr_planets = 3
        planets.object_id.expand(nr_planets)[:] = \
            np.arange(nr_planets)

        constants = planets.property_sets.add("constants")

        name = constants.add_property(
            "name", dtype=np.dtype(np.unicode_))
        name.value.expand(nr_planets)[:] = \
            np.array([
                    # Python 2: Byte string, encoded in UTF8
                    # Python 3: Unicode string
                    "ñeptùne",
                    "mærß",
                    "ùræñùß"
                ])

        gravity = constants.add_property(
            "gravity", dtype=np.dtype(np.float32))
        gravity.value.expand(nr_planets)[:] = \
            np.array([1.5, 2.5, 3.5], dtype=np.float32)

        lue.assert_is_valid(dataset)
