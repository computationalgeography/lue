import numpy as np
import lue


nr_areas = 10
rank = 2

dataset = lue.create_dataset("areas.lue")
area = dataset.add_phenomenon("area")

id = [2, 4, 6, 8, 10, 9, 7, 5, 3, 1]
area.object_id.expand(nr_areas)[:] = id

space_configuration = lue.SpaceConfiguration(
    lue.Mobility.stationary,
    lue.SpaceDomainItemType.box
)
constant = area.add_property_set(
    "constant", space_configuration,
    space_coordinate_dtype=np.dtype(np.float32), rank=rank)

box = np.arange(nr_areas * rank * 2, dtype=np.float32).reshape(
    nr_areas, rank * 2)
constant.space_domain.value.expand(nr_areas)[:] = box

# Property with differently shaped 2D object arrays
elevation_datatype = np.dtype(np.float32)
elevation = constant.add_property(
    "elevation", dtype=elevation_datatype, rank=rank)
count_datatype = lue.dtype.Count
shape = np.arange(  # Dummy data
    nr_areas * rank, dtype=count_datatype).reshape(nr_areas, rank)
value = elevation.value.expand(id, shape)

for a in range(nr_areas):
    value[id[a]][:] = (  # Dummy data
        10 * np.random.rand(*shape[a])).astype(elevation_datatype)

# Discretization property with 1D object arrays containing the shape of
# each object's elevation array: nr_rows and nr_cols
discretization = constant.add_property(
    "discretization", dtype=count_datatype, shape=(rank,))
discretization.value.expand(nr_areas)[:] = shape

elevation.set_space_discretization(
    lue.SpaceDiscretization.regular_grid, discretization)
