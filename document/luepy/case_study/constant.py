import numpy
import lue


# In this example we assume that the collection of items remains constant,
# and that the shape of all item-values is the same.
omnipresent = lue.constant_size.time.omnipresent

dataset = lue.create_dataset("planets.lue")
phenomenon = dataset.add_phenomenon("planets")
constants = omnipresent.create_property_set(phenomenon, "constants")

nr_planets = 3

value_type = numpy.float32
gravity = omnipresent.same_shape.create_property(
    constants, "gravity", value_type)
values = gravity.reserve(nr_planets)
values[:] = numpy.array([ 1.5, 2.5, 3.5 ], dtype=value_type)
