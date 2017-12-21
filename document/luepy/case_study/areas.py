import numpy
import lue


# In this example we assume that the collection of items remains constant
omnipresent = lue.constant_size.time.omnipresent

dataset = lue.create_dataset("areas.lue")
phenomenon = dataset.add_phenomenon("areas")
areas = omnipresent.create_property_set(phenomenon, "areas")

nr_areas = 10
rank = 2


# Per area a unique id
areas.ids.reserve(nr_areas)[:] = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]


# Space domain
space_domain = omnipresent.create_space_box_domain(
    areas, numpy.float64, rank)
boxes = space_domain.reserve(nr_areas)
nr_coordinates_per_box = boxes.shape[1]
boxes_ = numpy.arange(  # Dummy data...
    nr_areas * nr_coordinates_per_box, dtype=numpy.float64)
boxes_ = boxes_.reshape(nr_areas, nr_coordinates_per_box)
boxes[:] = boxes_


# Discretization property with two values per item: nr_rows, nr_cols
discretization = omnipresent.same_shape.create_property(
    areas, "space discretization", numpy.uint32, (rank,))
nr_cells = discretization.values.reserve(nr_areas)
nr_cells_ = numpy.arange(  # Dummy data...
    start=1, stop=nr_areas * rank + 1, dtype=numpy.uint32)
nr_cells_ = nr_cells_.reshape(nr_areas, rank)
nr_cells[:] = nr_cells_


# Elevation fields
elevation = omnipresent.different_shape.create_property(
    areas, "elevation", numpy.float64, rank)
values = elevation.values.reserve(nr_cells_)

for i in range(nr_areas):
    shape = nr_cells_[i]
    # Dummy data...
    values[i][:] = (10 * numpy.random.rand(*shape)).astype(numpy.float64)


# Link elevation property with discretization property
elevation.discretize_space(discretization)


lue.assert_is_valid(dataset)
