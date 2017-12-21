import numpy
import lue


# In this example we assume that the collection of items remains constant
omnipresent = lue.constant_size.time.omnipresent

dataset = lue.create_dataset("cities.lue")
phenomenon = dataset.add_phenomenon("cities")
cities = omnipresent.create_property_set(phenomenon, "cities")

nr_cities = 10
rank = 2


# Per area a unique id
cities.ids.reserve(nr_cities)[:] = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]


# Space domain
space_domain = omnipresent.create_space_point_domain(
    cities, numpy.float64, rank)
points = space_domain.reserve(nr_cities)
points_ = numpy.arange(  # Dummy data...
    nr_cities * rank, dtype=numpy.float64)
points_ = points_.reshape(nr_cities, rank)
points[:] = points_


# Property
population = omnipresent.same_shape.create_property(
    cities, "population", numpy.int64)
values = population.values.reserve(nr_cities)
# Dummy data...
values[:] = (numpy.random.rand(nr_cities) * 1e6).astype(numpy.int64)


lue.assert_is_valid(dataset)
