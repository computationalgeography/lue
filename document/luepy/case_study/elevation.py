import numpy
import lue


omnipresent = lue.constant_size.time.omnipresent
located = lue.constant_size.time.located

dataset = lue.create_dataset("elevation.lue")
phenomenon = dataset.add_phenomenon("areas")
areas = located.create_property_set(phenomenon, "areas")
nr_areas = 10

areas.ids.reserve(nr_areas)[:] = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]

# Time domain contains 1D boxes, with a resolution of 1 day
time_domain = located.create_time_box_domain(areas, lue.Clock(lue.unit.day, 1))
nr_time_boxes = 4
time_boxes = time_domain.reserve(nr_time_boxes)
# A box is defined by a begin and end time point (two coordinates per box)
# Here, we configure time boxes with a duration of 10 days. The time
# inbetween the boxes is time were we don't have information.
time_boxes[:] = numpy.array([  # Dummy data...
        [10,  20],
        [30,  40],
        [50,  60],
        [70,  80],
    ], time_boxes.dtype)

space_domain = located.create_space_box_domain(areas, numpy.float64, rank=2)
space_domain.reserve(nr_areas)[:] = numpy.arange(  # Dummy data...
    nr_areas * 4, dtype=numpy.float64).reshape(nr_areas, 4)

# Given the 10 day time boxes, configure time steps to be 2 days long
nr_time_steps = 5
discretization_phenomenon = dataset.add_phenomenon("discretization")
discretization_property_set = omnipresent.create_property_set(
    discretization_phenomenon, "discretization")
discretization_property_set.ids.reserve(1)[0] = 12345

time_discretization = omnipresent.same_shape.create_property(
    discretization_property_set, "time", numpy.uint32)
time_discretization.values.reserve(1)[:] = nr_time_steps

discretization_property_set = omnipresent.create_property_set(
    phenomenon, "discretization", areas.ids)
space_discretization = omnipresent.same_shape.create_property(
    discretization_property_set, "space", numpy.uint32, (2,))
nr_cells = numpy.arange(start=1, stop=nr_areas * 2 + 1, dtype=numpy.uint32)
nr_cells = nr_cells.reshape(nr_areas, 2)
space_discretization.values.reserve(nr_areas)[:] = nr_cells

# Re-use nr_cells array with shapes of spatial fields, but add information
# about the number of time steps. The resulting values will be
# [nr_time_steps, nr_rows, nr_cols], where nr_rows, nr_cols is different
# for each raster.
nr_cells = numpy.insert(nr_cells, 0, nr_time_steps, axis=1)

# Elevation fields
elevation = located.constant_shape.different_shape.create_property(
    areas, "elevation", numpy.float64, 3)
values = elevation.values.reserve(nr_time_boxes, nr_cells)
assert len(values) == nr_areas, len(values)

for i in range(nr_areas):
    shape = values[i].shape
    # [nr_time_domain_items, nr_time_steps, nr_rows, nr_cols]
    assert len(shape) == 4, shape
    values[i][:] = (10 * numpy.random.rand(*shape)).astype(numpy.float64)

elevation.discretize_space(space_discretization)
elevation.discretize_time(time_discretization)

lue.assert_is_valid(dataset)
