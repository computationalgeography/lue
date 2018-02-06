import numpy
import lue


omnipresent = lue.constant_collection.time.omnipresent

dataset = lue.create_dataset("cities.lue")
phenomenon = dataset.add_phenomenon("cities")
cities = lue.constant_collection.create_property_set(phenomenon, "cities")

nr_cities = 10
rank = 2

cities.ids.reserve(nr_cities)[:] = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]

# Space domain contains 2D points
space_domain = omnipresent.create_space_point_domain(
    cities, numpy.float64, rank)
points = space_domain.reserve(nr_cities)
points[:] = numpy.arange(  # Dummy data...
    nr_cities * rank, dtype=numpy.float64).reshape(nr_cities, rank)

# Property
population = omnipresent.same_shape.create_property(
    cities, "population", numpy.int64)
values = population.values.reserve(nr_cities)
# Dummy data...
values[:] = (numpy.random.rand(nr_cities) * 1e6).astype(numpy.int64)

lue.assert_is_valid(dataset)
