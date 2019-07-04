import numpy as np
import lue


nr_planets = 3

dataset = lue.create_dataset("planets.lue")
planet = dataset.add_phenomenon("planet")

planet.object_id.expand(nr_planets)[:] = [4, 29, 13]

constant = planet.add_property_set("constant")
gravity = constant.add_property("gravity", dtype=np.dtype(np.float32))

gravity.value.expand(nr_planets)[:] = \
    np.array([ 1.5, 2.5, 3.5 ], dtype=np.float32)
