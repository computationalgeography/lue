import numpy
import lue


# In this example we assume that the collection of items remains constant,
# and that the shape of all item-values is the same.
omnipresent = lue.constant_size.time.omnipresent

dataset = lue.create_dataset("planets.lue")
planets = dataset.add_phenomenon("planets")
constants = omnipresent.create_property_set(planets, "constants")

nr_planets = 3


# Per planet a unique id
constants.ids.reserve(nr_planets)[:] = [4, 29, 13]


# Gravity constants
gravity = omnipresent.same_shape.create_property(
    constants, "gravity", numpy.float32)
values = gravity.values.reserve(nr_planets)
# Dummy data...
values[:] = numpy.array([ 1.5, 2.5, 3.5 ], dtype=numpy.float32)


lue.assert_is_valid(dataset)
