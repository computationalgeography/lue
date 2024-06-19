# -*- encoding: utf-8 -*-
import numpy as np

import lue.data_model as ldm
import lue_test


class ConstantTest(lue_test.TestCase):
    def test_case_study(self):
        dataset = ldm.create_dataset("planets.lue")
        planets = dataset.add_phenomenon("planets")

        nr_planets = 3
        planets.object_id.expand(nr_planets)[:] = np.arange(nr_planets)

        constants = planets.add_property_set("constants")

        name = constants.add_property("name", dtype=np.str_)
        name.value.expand(nr_planets)[:] = np.array(["ñeptùne", "mærß", "ùræñùß"])

        gravity = constants.add_property("gravity", dtype=np.float32)
        gravity.value.expand(nr_planets)[:] = np.array(
            [1.5, 2.5, 3.5], dtype=np.float32
        )

        ldm.assert_is_valid(dataset)
