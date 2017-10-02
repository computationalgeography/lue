import numpy
import lue


# In this example we assume that the collection of items remains constant,
# and that the shape of all item-values is the same.
omnipresent = lue.constant_size.time.omnipresent

dataset = lue.create_dataset("planets.lue")
phenomenon = dataset.add_phenomenon("planets")
constants = omnipresent.create_property_set(phenomenon, "constants")

nr_planets = 3


# Per planet a unique id
ids = constants.reserve(nr_planets)
ids[:] = [4, 29, 13]


# Gravity constants
gravity = omnipresent.same_shape.create_property(
    constants, "gravity", numpy.float32)
values = gravity.reserve(nr_planets)
# Dummy data...
values[:] = numpy.array([ 1.5, 2.5, 3.5 ], dtype=numpy.float32)
